#include <stdint.h>
#include <string.h>

/**
 * Get the length of a possibly null terminated string, clamped to a maximum.
 *
 * If \p s is not NULL, searches up to \p maxsize bytes from \p s to find the
 * first null terminator, if any.
 *
 * \param s        Start of string.
 * \param maxsize  Maximum number of bytes to search.
 *
 * \return 0 if \p s is \c NULL.
 * \return \p maxsize if null terminator not found.
 * \return length of null terminated string if null terminator found.
 */
size_t safe_strlen(const char *s, size_t maxsize)
{
    size_t length = 0;
    if (s)
    {
#if PTRDIFF_MAX < SIZE_MAX
        /* May need to search the buffer in chunks. */
        while (maxsize)
#endif
        {
            const char *e;
            size_t pos;
#if PTRDIFF_MAX < SIZE_MAX
            if (maxsize > PTRDIFF_MAX)
            {
                /* Limit size of chunk. */
                pos = PTRDIFF_MAX;
            }
            else
#endif
            {
                /* This is the final chunk. */
                pos = maxsize;
            }
            /* Search for null terminator in chunk. */
            e = memchr(s, 0, pos);
            if (e) {
                /* Null terminator found. */
                pos = e - s;  /* position of null terminator in chunk */
#if PTRDIFF_MAX < SIZE_MAX
                /* Make this the final chunk. */
                maxsize = pos;
#endif
            }
            /* Update returned length. */
            length += pos;
#if PTRDIFF_MAX < SIZE_MAX
            /* Advance to next chunk. */
            s += pos;
            maxsize -= pos;
#endif
        }
    }
    return length;
}