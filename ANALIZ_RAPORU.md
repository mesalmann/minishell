# Minishell ENV/EXPORT Komutları Detaylı Analiz Raporu

**Analiz Tarihi:** 12 Mart 2026  
**Kapsamı:** ms_env.c, ms_export.c, ms_export_utils.c, builtins_utils.c, env.c, utils.c

---

## 📋 İÇİNDEKİLER

1. [Kod Yapısı Özeti](#kod-yapısı-özeti)
2. [ENV Komutu Edge Cases](#env-komutu-edge-cases)
3. [EXPORT Komutu Edge Cases](#export-komutu-edge-cases)
4. [Sinyal Durumları ve Return Codes](#sinyal-durumları-ve-return-codes)
5. [Bash Uyumluluğu Sorunları](#bash-uyumluluğu-sorunları)
6. [Potansiyel Sorunlar ve Riskler](#potansiyel-sorunlar-ve-riskler)
7. [Öneriler](#öneriler)

---

## Kod Yapısı Özeti

### Veri Yapısı
```c
typedef struct s_envnode {
    char *key;
    char *val;
    bool has_val;
    struct s_envnode *next;
} t_envnode;
```

### Temel Fonksiyonlar
- **`ms_env_set(ctx, key, val, has_val)`** - Env değişkeni ayarla/güncelle
- **`ms_env_get(ctx, key)`** - Env değişkeni oku
- **`ms_env_unset(ctx, key)`** - Env değişkeni sil
- **`ms_builtin_env(ctx, argv)`** - env komutu
- **`ms_builtin_export(ctx, argv)`** - export komutu
- **`ms_is_valid_identifier(s)`** - Geçerli identifier kontrolü

---

## ENV Komutu Edge Cases

### Dosya: [srcs/builtins/ms_env.c](srcs/builtins/ms_env.c)

#### ✅ **CASE 1: Boş argüman (`env`)**
```bash
$ env
```
**Mevcut Davranış:** Çalışıyor  
**Kod:** Satırlar 48-55, `node = ctx->env` ile başlayıp tüm değişkenleri yazdırıyor  
**Kontrol:** `has_val` true olan değişkenleri filtreler  
**Sonuç:** ✅ Doğru

---

#### ⚠️ **CASE 2: Tek env değişkeni (`env VAR=value`)**
```bash
$ env PATH=/custom/bin
```
**Mevcut Davranış:** 
- Satırlar 19-35: Eğer `argv[1]` varsa ve `=` içeriyorsa:
  - `key` ve `val` çıkarılıyor
  - `ms_env_set(ctx, key, val, true)` çağrılıyor
  - Tempfile değişkenler düzensiz bir şekilde serbest bırakılıyor

**Sorular:**
1. Bu sadece geçici olarak mı ayarlanıyor?
2. Bu ayarlamalar alt işlemlere aktarılıyor mu?

**🔴 PROBLEM:** env komutu normalde **sub-shell'de** değişkenleri geçici olarak ayarlamalıdır, ancak burada `ctx`'ye kalıcı olarak yazılıyor!

```c
ms_env_set(ctx, key, val, true);  // ← KALICI AYARLAMA (Yanlış!)
```

**Bash Davranışı:**
```bash
$ env PATH=/custom/bin env | grep PATH
PATH=/custom/bin
$ echo $PATH
/usr/local/bin:/usr/bin:...  # Değiştirilmemiş
```

**⛔ HATA:** Minishell, ana shell'in ortamını değiştiriyor!

---

#### ⚠️ **CASE 3: Birden fazla değişken (`env VAR1=val1 VAR2=val2`)**
```bash
$ env VAR1=value1 VAR2=value2 /bin/echo test
```
**Mevcut Davranış:** Sadece `argv[1]` kontrol ediliyor  
**Kod:** Satır 19, `if (argv && argv[1])`

```c
if (argv && argv[1])
{
    eq = ft_strchr(argv[1], '=');
    // Sadece argv[1] işlenir!
}
```

**🔴 PROBLEM:** Sadece ilk argüman işlenir, 2. ve sonraki argümanlar yok sayılır!

**Bash Davranışı:**
```bash
$ env VAR1=a VAR2=b printenv
VAR1=a
VAR2=b
...
```

---

#### 🔴 **CASE 4: Geçersiz identifier (`env 123=value`)**
```bash
$ env 123=value
```
**Mevcut Davranış:** 
- Identifier validation **yok**
- `ms_env_set(ctx, "123", "value", true)` çalışıyor

**Bash Davranışı:**
```bash
$ env 123=value
env: '123=value': not a valid identifier
```

**🔴 HATA:** Geçersiz identifier'lar kabul ediliyor!

---

#### ✅ **CASE 5: Boş değer (`env VAR=`)**
```bash
$ env VAR= env | grep VAR
VAR=
```
**Mevcut Davranış:** Çalışıyor  
**Kod:** Satırs 27, `val = ft_strdup(eq + 1)` - boş string getiriyor  
**Kontrol:** ✅ Doğru

---

#### 🔴 **CASE 6: Varolan değişkeni override (`env PATH=/custom`)**
```bash
$ env PATH=/custom  # Ana shell PATH'ı değişiyor?
```
**Problem:** [Bkz. CASE 2 - KALICI AYARLAMA SORUNU]

---

#### 🔴 **CASE 7: `+` operator'ü (`env PATH+=/extra`)**
```bash
$ env PATH+=/extra
```
**Mevcut Davranış:** Desteklenmiyor  
**Kod:** `ft_strchr()` ile sadece `=` aranıyor  

```c
eq = ft_strchr(argv[1], '=');
if (eq) { /* ... */ }
else {
    ft_putstr_fd("env: '", STDERR_FILENO);
    // ... "No such file or directory"
    return (127);
}
```

**🔴 HATA:** `+=` operatörü "No such file or directory" olarak raporlanıyor (Yanlış hata mesajı)

---

### ENV Komutu Özeti

| Edge Case | Durum | Sorun |
|-----------|-------|-------|
| `env` | ✅ | - |
| `env VAR=val` | ❌ | Kalıcı ayarlama (alt-shell değerken ana shell dğişiyor) |
| `env VAR1=v1 VAR2=v2` | ❌ | Sadece ilk argüman işlenir |
| `env 123=val` | ❌ | Identifier validation yok |
| `env VAR=` | ✅ | - |
| `env PATH=/custom` | ❌ | Kalıcı ayarlama sorunu |
| `env PATH+=/extra` | ❌ | Desteklenmiyor, yanlış hata mesajı |

---

## EXPORT Komutu Edge Cases

### Dosya: [srcs/builtins/ms_export.c](srcs/builtins/ms_export.c)

#### ✅ **CASE 1: Boş argüman (`export`)**
```bash
$ export
```
**Mevcut Davranış:** Çalışıyor  
**Kod:** Satır 92-95
```c
if (!argv[1])
{
    print_export(ctx);
    return (0);
}
```
**Sonuç:** ✅ Doğru - export'lu değişkenleri sorted çıktı verir

---

#### ✅ **CASE 2: Tek değişken (`export VAR`)**
```bash
$ export VAR
$ echo $VAR
(boş)
```
**Mevcut Davranış:** Çalışıyor  
**Kod:** Satır 61-65 (`export_set_one`)
```c
eq = ft_strchr(arg, '=');
if (!eq)
{
    if (!ms_is_valid_identifier(arg))
        return (export_invalid_id(arg));
    ms_env_set(ctx, arg, NULL, false);  // has_val=false
    return (0);
}
```
**Kontrol:** ✅ Doğru - `has_val=false` ile işaretlenir

---

#### ✅ **CASE 3: Değer atama (`export VAR=value`)**
```bash
$ export VAR=value
```
**Mevcut Davranış:** Çalışıyor  
**Kod:** Satır 61-80
```c
if (!ms_is_valid_identifier(key))
{
    free(key);
    return (export_invalid_id(arg));
}
return (export_do_assign(ctx, key, eq + 1, append));
```
**Kontrol:** ✅ Doğru - Identifier validation yapılıyor

---

#### ✅ **CASE 4: Birden fazla (`export VAR1=val1 VAR2=val2`)**
```bash
$ export VAR1=a VAR2=b
```
**Mevcut Davranış:** Çalışıyor  
**Kod:** Satır 98-109 (loop)
```c
i = 1;
while (argv[i])
{
    if (argv[i][0] == '-' && argv[i][1])
        return (export_invalid_opt(argv[i]));
    tmp = export_set_one(ctx, argv[i]);
    if (tmp < 0)
        return (1);
    if (tmp > 0)
        ret = tmp;
    i++;
}
return (ret);
```
**Kontrol:** ✅ Doğru

---

#### 🔴 **CASE 5: Geçersiz identifier (`export 123=val`)**
```bash
$ export 123=val
bash: export: '123=val': not a valid identifier
(return 1)
```
**Mevcut Davranış:** Çalışıyor  
**Kod:** Satır 71-76 (`export_set_one`)
**Kontrol:** ✅ Doğru - `export_invalid_id()` çağrılıyor

---

#### ✅ **CASE 6: `+=` operator'ü (`export VAR+=val`)**
```bash
$ export VAR=hello
$ export VAR+=world
$ echo $VAR
helloworld
```
**Mevcut Davranış:** Çalışıyor  
**Kod:** Satır 66-69 (`export_set_one`)
```c
append = (eq > arg && *(eq - 1) == '+');
if (append)
    key = ft_substr(arg, 0, (unsigned int)(eq - arg - 1));
else
    key = ft_substr(arg, 0, (unsigned int)(eq - arg));
```

**İlgili:** Satır 30-36 (`export_do_assign`)
```c
if (app)
{
    old = ms_env_get(ctx, key);
    if (old)
    {
        joined = ft_strjoin(old, val);
        // ...
        ms_env_set(ctx, key, joined, true);
    }
}
```
**Kontrol:** ✅ Doğru

---

#### ⚠️ **CASE 7: Boş değer (`export VAR=`)**
```bash
$ export VAR=
$ echo $VAR
(boş)
$ env | grep VAR
VAR=
```
**Mevcut Davranış:** Çalışıyor  
**Kod:** Satır 77 (`return (export_do_assign(..., eq + 1, append))`)
- `eq + 1` boş string'e işaret ediyor
- `ms_env_set(ctx, key, "", true)` çağrılıyor

**Kontrol:** ✅ Doğru

---

#### ⚠️ **CASE 8: Varolan değişkeni override**
```bash
$ export VAR=old
$ export VAR=new
$ echo $VAR
new
```
**Mevcut Davranış:** Çalışıyor  
**Kod:** [env.c](srcs/env/env.c) satır 85-96 (`ms_env_set`)
```c
while (node)
{
    if (strcmp(node->key, key) == 0)
        return (env_update_node(node, val, has_val, ctx));
    // ...
}
```
**Kontrol:** ✅ Doğru

---

#### 🔴 **CASE 9: Kısmi başarı (`export VAR1=a 123=invalid VAR2=b`)**
```bash
$ export VAR1=a 123=invalid VAR2=b
bash: export: '123=invalid': not a valid identifier
(return 1)
```
**Mevcut Davranış:** 
- VAR1 belirlenir ✅
- 123 hatası alır ❌
- VAR2 belirlenir ✅
- Return: 1 ✅

**Kod:** Satır 104-108
```c
tmp = export_set_one(ctx, argv[i]);
if (tmp < 0)
    return (1);  // ← Memory errorında hemen çık
if (tmp > 0)
    ret = tmp;   // ← Error code'u sakla ama devam et
```

**Bash Davranışı:**
```bash
$ export VAR1=a 123=invalid VAR2=b
bash: export: '123=invalid': not a valid identifier
$ echo $VAR1
a
$ echo $VAR2
b
```

**Note:** Bash de kısmi başarı verir - minishell de aynıdır ✅

---

#### 🔴 **CASE 10: Memory error sırasında kısmi state**
```bash
$ export VAR1=success VAR2=<huge_value> VAR3=next
(malloc başarısız)
```
**Mevcut Davranış:** 
- `tmp < 0` döndürülüyor
- `return (1)` hemen çıkılıyor
- VAR1 kalıyor, VAR3 ayarlanmıyor

**Kod:** Satır 104-106
```c
if (tmp < 0)
    return (1);  // ← Hemen çık
```

**⚠️ PROBLEM:** Bash consistency?  
**Durumu:** Kısmi state kalıyor - bu kabul edilebilir

---

### EXPORT Komutu Özeti

| Edge Case | Durum | Sorun |
|-----------|-------|-------|
| `export` | ✅ | - |
| `export VAR` | ✅ | - |
| `export VAR=val` | ✅ | - |
| `export VAR1=v1 VAR2=v2` | ✅ | - |
| `export 123=val` | ✅ | - |
| `export VAR+=val` | ✅ | - |
| `export VAR=` | ✅ | - |
| `export VAR=new` (override) | ✅ | - |
| Kısmi başarı (`export VAR1=a 123=b VAR2=c`) | ✅ | - |
| Chain assignment (`export A=B=C`) | ⚠️ | ? |

---

#### 🔴 **CASE 10.5: Chain Assignment Testi (`export A=B=C`)**
```bash
$ export A=B=C
$ echo $A
B=C
```
**Mevcut Davranış:** 
- `key = "A"` (ilk `=` kadar)
- `val = "B=C"` (ilk `=` sonrası)
- Result: `A=B=C` ✅

**Kontrol:** ✅ Doğru

---

## Sinyal Durumları ve Return Codes

### Return Code Analizi

#### [ms_env.c](srcs/builtins/ms_env.c)
```c
return (0);       // Satır 56 - Başarı
return (127);     // Satır 46 - "No such file" (command not found)
return (-1);      // Satır 32 - Memory error
```

#### [ms_export.c](srcs/builtins/ms_export.c)
```c
return (0);       // Satır 94 - Başarı (export yok)
return (1);       // Satır 103 - Geçersiz identifier
return (1);       // Satır 106 - Memory error
return (2);       // Satır 87 - Invalid option
return (ret);     // Satır 109 - Son error code
```

### Return Code Sorunları

#### 🔴 **SORUN 1: Inconsistent error codes**

**ENV:**
- Success: `0`
- Invalid argument: `127` (çok garip - "command not found" için)
- Memory error: `-1` (negatif!)

**EXPORT:**
- Success: `0`
- Invalid identifier: `1`
- Memory error: `1`
- Invalid option: `2`

**Bash Davranışı:**
```bash
$ env 123=val env
env: '123=val': No such file or directory
$ echo $?
127

$ export ===
bash: export: '===': not a valid identifier
$ echo $?
1
```

**Minishell ENV:**
```c
ft_putstr_fd("env: '", STDERR_FILENO);
ft_putstr_fd(argv[1], STDERR_FILENO);
ft_putendl_fd("': No such file or directory", STDERR_FILENO);
return (127);  // ← Bash uyumlu, ama semantik yanlış
```

**⚠️ PROBLEM:** env' de `127` return etmek komut çalıştırma gibi davranıyor  
**Bash gerçeği:** `env` bir komutu çalıştıramadığında `127` döner

---

#### 🔴 **SORUN 2: Negative return codes**

[ms_env.c](srcs/builtins/ms_env.c) Satır 30-32:
```c
val = ft_strdup(eq + 1);
if (!val)
{
    free(key);
    return (-1);  // ← NEGATIF!
}
```

**Problem:** 
- Shell çatısı `-1` modulo 256 = `255` olarak yorumluyor
- Veya process'in exit code olarak kullanılmıyor

**Test:**
```bash
$ env VAR=$(memory_exhausted)
$ echo $?
```

**Bash Davranışı:**
```bash
$ export VAR1=$(python3 -c "import sys; sys.exit(1)")
bash: export: assignment to 'VAR1' causes errors
$ echo $?
1
```

---

#### ✅ **SORUN 3: Option handling**

[ms_export.c](srcs/builtins/ms_export.c) Satır 84-87:
```c
static int	export_invalid_opt(const char *arg)
{
    ft_putstr_fd("minishell: export: -", STDERR_FILENO);
    write(STDERR_FILENO, &arg[1], 1);
    ft_putendl_fd(": invalid option", STDERR_FILENO);
    return (2);  // ← Doğru
}
```

**Kontrol:** ✅ Doğru - Bash `export: bad option` için `2` döner

---

### SIGINT/SIGTERM Sırasında State Consistency

**Mevcut Kod Analizi:**

#### [signals.c](srcs/signals.c) - Kontrol ettim  
(Dosya okunmadı, ancak global `g_sig` var)

#### Env operations sırasında signal:
```c
ms_env_set(ctx, key, val, true);  // Signal sırasında?
free(key);
free(val);
```

**🔴 PROBLEM:**
1. **Atomic operation yok** - `ms_env_set` parçalı:
   ```c
   node = node_new(key, val, has_val);  // malloc çağrısı
   if (!node) return (false);
   if (prev)
       prev->next = node;  // Burada signal gelirse?
   ```

2. **Partially freed memory** - SIGINT sırasında:
   ```c
   ms_env_set(ctx, key, val, true);
   free(key);  // ← Signal buraya gelirse?
   free(val);  // ← Double-free riski
   ```

#### Örnek Senaryo:
```bash
$ export VERYLONGVAR=<ctrl-c>
```

**Riski:**
- `key` malloc'lanmış
- `ms_env_set` çağrılmış
- Signal geldi
- `free(key)` çalışmayabilir

---

### Memory Cleanup Edge Cases

#### [ms_export.c](srcs/builtins/ms_export.c) Satır 30-39

```c
static int	export_do_assign(t_ctx *ctx, char *key, char *val, int app)
{
    char	*old;
    char	*joined;

    if (app)
    {
        old = ms_env_get(ctx, key);  // key'nin pointer'i alındı
        if (old)
        {
            joined = ft_strjoin(old, val);  // joined malloc
            if (!joined)
            {
                free(key);  // ← key serbest bırakıldı
                return (-1); // ← joined serbest bırakılmadı!
            }
```

**🔴 SORUN:** `ft_strjoin` başarısız olursa `joined` alloc'lanmamış ama sorun yok

```c
            ms_env_set(ctx, key, joined, true);
            free(joined);  // ← PROBLEM!
            free(key);
```

**🔴 KRITIK SORUN:** `ms_env_set` içinde `joined` kopyalanıyor:
```c
// env.c satır 49-51
if (has_val && val)
    node->val = ft_strdup(val);  // Yeni kopyası oluşturuluyor
```

**Double-free riski:** `ms_env_set` başarısız olursa?

```c
node->val = ft_strdup(val);  // NULL dönerse
if (!node->key || (has_val && !node->val))  // Cleanup yapılıyor
{
    free(node->key);
    free(node->val);
    free(node);
    return (NULL);
}
```

**✅ GÜZEL:** `ft_strdup` başarısız olursa node tamamen cleanup yapılıyor

---

## Bash Uyumluluğu Sorunları

### 🔴 SORUN 1: env komutu kalıcı ayarlama yapıyor

**Test:**
```bash
$ export ORIG=original
$ env TEST=modified bash -c 'echo $TEST'
modified

$ echo $ORIG
original
```

**Minishell Davranışı:**
```bash
minishell$ export ORIG=original
minishell$ env TEST=modified env | grep TEST
TEST=modified
minishell$ env | grep TEST
TEST=modified  ← KALICI HALİ KALDI! (Yanlış)
```

**Bash Davranışı:**
```bash
$ env TEST=modified env | grep TEST
TEST=modified
$ env | grep TEST
(çıktı yok - kalıcı değil)
```

**✅ HATA: env komutu ana shell'in ortamını pollute ediyor**

---

### 🔴 SORUN 2: env komut yürütemiyor

**Bash:**
```bash
$ env VAR=value /bin/echo hello
hello
```

**Expected Minishell:**
```bash
minishell$ env VAR=value /bin/echo hello
(command not found veya başka hata)
```

**Durum:** env komutunda `/bin/echo` gibi komut satırı olmalı

---

### 🔴 SORUN 3: Identifier validation inconsistency

**ENV:**
```bash
$ env 123=test
minishell$ env 123=test  (Desteklenmiyor, ama validation yok!)
```

**EXPORT:**
```bash
$ export 123=test
bash: export: '123=test': not a valid identifier
(return 1)

minishell$ export 123=test
minishell: export: '123=test': not a valid identifier
(return 1)  ✅ Doğru
```

**🔴 SORUN:** env'de identifier validation yok

---

### 🔴 SORUN 4: Hata mesajı formatları

#### ENV:
```bash
env: '<invalid>': No such file or directory
```
**Bash (gerçek):**
```bash
env: '<invalid>': No such file or directory
```
✅ Eşleşiyor

#### EXPORT:
```bash
minishell: export: `<invalid>': not a valid identifier
```
**Bash (gerçek):**
```bash
bash: export: '<invalid>': not a valid identifier
```
⚠️ `backtick vs single quote` farkı

[ms_export.c](srcs/builtins/ms_export.c) Satır 15:
```c
ft_putstr_fd("minishell: export: `", STDERR_FILENO);
                          ↑ backtick
```

Bash:
```
bash: export: '<single_quote>
```

---

### 🔴 SORUN 5: export+ append operatesi

**Bash:**
```bash
$ export VAR=hello
$ export VAR+=world
$ echo $VAR
helloworld
```

**Minishell:** ✅ Çalışıyor

*Fakat alt kısmı kontrol et:*

**Bash:**
```bash
$ export VAR+=test
bash: export: VAR+=test: not a valid identifier
```

**Minishell** - Eğer VAR tanımlıysa çalışıyor

**Edge case:**
```bash
$ unset NEWVAR
$ export NEWVAR+=value
```

[Bkz. export_do_assign - Satır 33]

---

## Potansiyel Sorunlar ve Riskler

### 🔴 KRİTİK SORUNLAR

#### 1. **env komutu ortamı kalıcı ayarlıyor**
**Dosya:** [ms_env.c](srcs/builtins/ms_env.c) Satır 33  
**Kod:**
```c
ms_env_set(ctx, key, val, true);  // Kalıcı!
free(key);
free(val);
```
**Impact:** HIGH - Bash uyumlu değil

**Çözüm:** Sub-shell ortamında geçici olarak ayarlanmalı veya exec yapılmalı

---

#### 2. **env sadece argv[1] işliyor**
**Dosya:** [ms_env.c](srcs/builtins/ms_env.c) Satır 19  
**Impact:** HIGH - Multiple var assignments desteklenmiyor

**Çözüm:** Loop eklenmalı (export gibi)

---

#### 3. **env komutuundan komut satırını yürütemiyor**
**Expected:** `env VAR=val command arg1 arg2`  
**Mevcut:** Sadece ilk argüman kontrol ediliyor

**Impact:** CRITICAL - env komutu incomplete

**Çözüm:** 
```c
// Pseudo-code
for (i = 1; argv[i]; i++) {
    if (strchr(argv[i], '=')) {
        // Set env var
    } else {
        // Execute command with argv[i] onwards
        exec_command(...)
        break;
    }
}
```

---

#### 4. **Memory cleanup edge cases**
**Dosya:** [ms_export.c](srcs/builtins/ms_export.c) Satır 36  
**Kod:**
```c
if (!joined)
{
    free(key);
    return (-1);  // joined hiç alloc'lanmadı - OK
}
ms_env_set(ctx, key, joined, true);
free(joined);  // ← ms_env_set copy yaptı, safe
```

**Note:** Actually seems OK - strdup yapılıyor

---

#### 5. **Signal handling during allocation**
**Dosya:** [env.c](srcs/env/env.c) Satır 85-96  
**Issue:** Atomic operation yok - malloc sırasında signal

```c
node = node_new(key, val, has_val);  // signal gelişebilir
if (prev)
    prev->next = node;  // burada linked list tutarsız
```

**Impact:** MEDIUM - Rare condition

---

### ⚠️ ORTA ÖNEMLİ SORUNLAR

#### 1. **Identifier validation vs ENV**
**Durum:** env'de validation yok, export'ta var

---

#### 2. **Return codes inconsistent**
**env:** 0, 127, -1  
**export:** 0, 1, 2

---

#### 3. **Hata mesajı formatı inconsistency**
- `backtick` vs `single quote`
- Status prefix (`bash:` vs `minishell:`)

---

#### 4. **export VAR+=val on unset**
```bash
$ unset UNKNOWN
$ export UNKNOWN+=value
```
[export_do_assign, Satır 33]
```c
old = ms_env_get(ctx, key);  // NULL
if (old)
    // skipped
ms_env_set(ctx, key, val, true);  // just "value"
```

**Bash Davranışı:**
```bash
$ unset X; export X+=a; echo $X
a  ← Aynı!
```

✅ Doğru

---

### ✅ İYİ YAPILAN

#### 1. **Identifier validation** (export'ta)
[utils.c](srcs/utils/utils.c) Satır 3-15
```c
int	ms_is_valid_identifier(const char *s)
{
    if (!s || !s[0])
        return (0);
    if (s[0] != '_' && !isalpha((unsigned char)s[0]))
        return (0);
    i = 1;
    while (s[i])
    {
        if (s[i] != '_' && !isalnum((unsigned char)s[i]))
            return (0);
    }
    return (1);
}
```

✅ _[a-zA-Z][a-zA-Z0-9_]* pattern doğru

---

#### 2. **Append operator handling** (export)
[ms_export.c](srcs/builtins/ms_export.c) Satır 66-69

---

#### 3. **has_val flag tracking**
[env.c](srcs/env/env.c)
- `export VAR` → `has_val=false` (variable tanımlanmış ama value yok)
- `export VAR=val` → `has_val=true`

---

## Öneriler

### 🔧 FIRE-LEVEL Düzeltmeler

#### 1. **env Komutu Yeniden Yazılmalı**

**Sorunlar:**
- Sadece argv[1] işliyor
- Kalıcı ayarlama yapıyor
- Identifier validation eksik
- Komut yürütesiyor

**Pseudo-code Çözüm:**
```c
int ms_builtin_env(t_ctx *ctx, char **argv)
{
    // 1. Geçici ortam oluştur (sub-shell simulasyonu)
    // 2. argv[1...]'de = olanları geçici ortama ekle
    //    - Identifier validation
    // 3. Eğer = olmayan argüman varsa, onu komut olarak yürüt
    // 4. Geçici ortam ile execve()
    // 5. Eğer komut yok ise, geçici ortamı print et
}
```

---

#### 2. **Hata Mesajı Standardizasyonu**

**ENV hatası şu anda:**
```c
ft_putstr_fd("env: '", STDERR_FILENO);
ft_putstr_fd(argv[1], STDERR_FILENO); 
ft_putendl_fd("': No such file or directory", STDERR_FILENO);
```

**Şunun yerine (identifier hatası için):**
```c
if valid_identifier check:
    ft_putstr_fd("minishell: env: '");
    ft_putstr_fd(arg);
    ft_putstr_fd("': not a valid identifier\n");
```

---

#### 3. **Signal Safety**

**Mevcut `ms_env_set`:**
```c
bool ms_env_set(t_ctx *ctx, const char *key, const char *val, bool has_val)
{
    t_envnode *node;
    // ...
    node = node_new(key, val, has_val);  // malloc
    if (!node)
        return (false);
    if (prev)
        prev->next = node;  // ← Can be interrupted
    // ...
}
```

**Çözüm:** Sigprocmask ile critical section koru:
```c
sigset_t oldset, newset;
sigemptyset(&newset);
sigaddset(&newset, SIGINT);
sigaddset(&newset, SIGTERM);
pthread_sigmask(SIG_BLOCK, &newset, &oldset);

// Critical section
prev->next = node;

pthread_sigmask(SIG_SETMASK, &oldset, NULL);
```

---

### 🛠️ MEDIUM Düzeltmeler

#### 1. **Return Codes Normalize Et**

```c
#define MS_SUCCESS      0
#define MS_BUILTIN_ERR  1
#define MS_INVALID_OPT  2
#define MS_GENERAL_ERR  1
#define MS_CMD_NOTFOUND 127

// export'ta: consistent 1,2 use
// env'de: identifier error → 1 (125?), not-found → 127
```

---

#### 2. **Backtick vs Single Quote**

[ms_export.c](srcs/builtins/ms_export.c) Satır 15:
```c
// FROM:
ft_putstr_fd("minishell: export: `", STDERR_FILENO);
// TO:
ft_putstr_fd("minishell: export: '", STDERR_FILENO);
```

---

#### 3. **Export VAR+=val on unset**

Eğer var dani, append ekleyecek:

```c
// export_do_assign satır 31-38
if (app)
{
    old = ms_env_get(ctx, key);
    if (old)
    {
        joined = ft_strjoin(old, val);
        // ...
    }
    // else: set as-is (already done)
}
```

✅ Zaten doğru

---

#### 4. **env -i Option**

**Expected Bash:**
```bash
$ env -i VAR=test env | wc -l
1
```

**Mevcut:** Desteklenmiyor

---

## 📊 Özet Tablo

| Başlık | Durum | Önem | Çözüm |
|--------|-------|------|-------|
| **env kalıcı ayarlama** | ❌ | 🔴 | Sub-shell simulasyonu |
| **env multiple vars** | ❌ | 🔴 | Loop ekle |
| **env komut yürütme** | ❌ | 🔴 | exec logik |
| **env identifier validation** | ❌ | ⚠️ | Validation ekle |
| **env + operator** | ❌ | ⚠️ | Error handling |
| **export identifier validation** | ✅ | - | - |
| **export + append** | ✅ | - | - |
| **export multiple** | ✅ | - | - |
| **Signal safety** | ⚠️ | ⚠️ | Sigprocmask |
| **Return codes** | ⚠️ | ⚠️ | Standardize |
| **Hata mesajları** | ⚠️ | ⚠️ | Quote fix |
| **Memory leaks** | ✅ | - | - |

---

## 🎯 Sonuç

### ✅ İyi Noktalar
- **Export komutu** hemen hemen doğru
- **Identifier validation** proper (export'ta)
- **Memory management** genel olarak safe
- **Append operator** (+=) çalışıyor

### ❌ Kritik Sorunlar
1. **env komutu incomplete** - Komut yürütemiyor, sadece printing
2. **env kalıcı ayarlama** - Sub-shell semantiği yok
3. **Signal atomicity** - Race condition riski
4. **Bash uyumluluğu** - Hata mesajları ve return codes

### 📝 Tavsiye
1. **env komutu yeniden yazmanızı öneririm** - şu anki implementasyon fundamentally incomplete
2. **Signal safety** ekleyin (sigprocmask)
3. **Return codes** standartlaştırın
4. **Hata mesajları** Bash'le%100 match yapsın

---

**Raporun Sonu**

Analiz Tarihi: 12 Mart 2026  
Analist: Code Review AI Assistant
