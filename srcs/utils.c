#include "minishell.h"

size_t ft_strlen(const char *s)
{
    size_t i = 0;
    while (s[i]) i++;
    return i;
}

char *ft_strdup(const char *s1)
{
    char *dup;
    size_t len = ft_strlen(s1) + 1;
    dup = malloc(len);
    if (dup)
        ft_memset(dup, 0, len); 
    if (dup)
    {
        size_t i = 0;
        while (s1[i]) { dup[i] = s1[i]; i++; }
    }
    return dup;
}

int ft_strncmp(const char *s1, const char *s2, size_t n)
{
    size_t i = 0;
    while (i < n && (s1[i] || s2[i]))
    {
        if (s1[i] != s2[i])
            return ((unsigned char)s1[i] - (unsigned char)s2[i]);
        i++;
    }
    return 0;
}

char *ft_strchr(const char *s, int c)
{
    while (*s)
    {
        if (*s == (char)c) return (char *)s;
        s++;
    }
    if ((char)c == '\0') return (char *)s;
    return NULL;
}

void *ft_memset(void *b, int c, size_t len)
{
    unsigned char *ptr = b;
    while (len-- > 0)
        *ptr++ = c;
    return b;
}

char *ft_strjoin(char const *s1, char const *s2)
{
    char *res;
    size_t len1 = ft_strlen(s1);
    size_t len2 = ft_strlen(s2);
    
    res = malloc(len1 + len2 + 1);
    if (!res) return NULL;
    
    size_t i = 0;
    while (s1[i]) { res[i] = s1[i]; i++; }
    size_t j = 0;
    while (s2[j]) { res[i + j] = s2[j]; j++; }
    res[i + j] = '\0';
    return res;
}

char *ft_substr(char const *s, unsigned int start, size_t len)
{
    char *sub;
    size_t i;

    if (!s) return NULL;
    if (ft_strlen(s) < start) return ft_strdup("");
    
    if (ft_strlen(s + start) < len)
        len = ft_strlen(s + start);
        
    sub = malloc(len + 1);
    if (!sub) return NULL;
    
    i = 0;
    while (i < len && s[start + i])
    {
        sub[i] = s[start + i];
        i++;
    }
    sub[i] = '\0';
    return sub;
}

void free_tab(char **tab)
{
    int i = 0;
    if (!tab) return;
    while (tab[i])
    {
        free(tab[i]);
        i++;
    }
    free(tab);
}