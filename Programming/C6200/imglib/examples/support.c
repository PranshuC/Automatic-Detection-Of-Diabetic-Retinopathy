#include <string.h>
#include <stdio.h>
/* ======================================================================== */
/*  MEM_COMPARE -- Compare two memory images and output a detailed dump if  */
/*                 the images differ.                                       */
/* ======================================================================== */
int mem_compare(const void *r1, const char *n1, 
                const void *r2, const char *n2, int len)
{
    unsigned char *c1, *c2;
    int i, j, k, total, skip;
    unsigned addr;
    char buf[80];
    const char hex[16] = "0123456789ABCDEF";

    /* -------------------------------------------------------------------- */
    /*  Do a simple memcmp.  If it succeeds, return "OK".                   */
    /* -------------------------------------------------------------------- */
    if (memcmp(r1, r2, len) == 0)
        return 0;

    /* -------------------------------------------------------------------- */
    /*  The memcmp failed, so dump out a nicely formatted table which       */
    /*  shows both arrays, and notes where they differ.                     */
    /* -------------------------------------------------------------------- */
    printf("\n\nERROR:  Arrays '%s' and '%s' do not match.\n", n1, n2);

    printf("           %-32s  %-32s\n", n1, n2);
#ifdef _BIG_ENDIAN
    printf("Address     0   1   2   3   4   5   6   7"
                  "     0   1   2   3   4   5   6   7\n");
#else
    printf("Address     7   6   5   4   3   2   1   0"
                  "     7   6   5   4   3   2   1   0\n");
#endif
    printf("========   =============================="
                  "    ==============================\n");

    /* -------------------------------------------------------------------- */
    /*  Generate char ptrs to the arrays.                                   */
    /* -------------------------------------------------------------------- */
    c1 = (unsigned char *)r1;
    c2 = (unsigned char *)r2;

    /* -------------------------------------------------------------------- */
    /*  The output dump is aligned on 8-byte boundaries.  Handle arrays     */
    /*  which start on other boundaries by backing up the pointers a bit.   */
    /*  We'll output whitespace for the entries which are outside our       */
    /*  array.                                                              */
    /* -------------------------------------------------------------------- */
    addr  = (unsigned)&c1[0];
    skip  = addr & 7;
    addr -= skip;
    c1   -= skip;
    c2   -= skip;
    total = len + skip;

    /* -------------------------------------------------------------------- */
    /*  Initialize our line buffer to all blanks.  We'll fill in fields w/  */
    /*  hex characters and asterisks (to denote differences) as needed.     */
    /* -------------------------------------------------------------------- */
    memset(buf, ' ', sizeof(buf));
    buf[ 8] = ':';
    buf[ 9] = ' ';
    buf[10] = ' ';
    buf[76] = '\n';

    /* -------------------------------------------------------------------- */
    /*  Generate the output table.  Each line contains 8 bytes from each    */
    /*  array, plus the address of the first array corresponding to the     */
    /*  elements shown.  Asterisks are shown in the first array to          */
    /*  illustrate differences between it and the second array.             */
    /* -------------------------------------------------------------------- */
    for (i = 0; i < total; i += 8)
    {
        /* ---------------------------------------------------------------- */
        /*  "Render" the bytes within a line.                               */
        /* ---------------------------------------------------------------- */
        k = total - i;
        k = k > 8 ? 8 : k;

        for (j = skip; j < k; j++)
        {
            int val1 = c1[i + j], val2 = c2[i + j];
            int v1d1, v1d0, v2d1, v2d0;

            v1d1 = hex[0xF & (val1 >> 4)];
            v1d0 = hex[0xF &  val1      ];
            v2d1 = hex[0xF & (val2 >> 4)];
            v2d0 = hex[0xF &  val2      ];

#ifdef _BIG_ENDIAN
            buf[11 + 4*j] = v1d1;
            buf[12 + 4*j] = v1d0;
            buf[13 + 4*j] = val1 == val2 ? ' ' : '*';
            buf[45 + 4*j] = v2d1;
            buf[46 + 4*j] = v2d0;
#else
            buf[39 - 4*j] = v1d1;
            buf[40 - 4*j] = v1d0;
            buf[41 - 4*j] = val1 == val2 ? ' ' : '*';
            buf[73 - 4*j] = v2d1;
            buf[74 - 4*j] = v2d0;
#endif
        }


        /* ---------------------------------------------------------------- */
        /*  If this line did not contain 8 bytes from the array because we  */
        /*  are at one end or the other, blank out the unused bytes.        */
        /* ---------------------------------------------------------------- */
#ifdef _BIG_ENDIAN
        if (skip > 0)
        {
            memset(buf + 11, ' ', 4*skip);
            memset(buf + 45, ' ', 4*skip);
        }
        if (k < 8) 
        {
            memset(buf + 11 + 4*k, ' ', 32 - 4*k);
            memset(buf + 45 + 4*k, ' ', 32 - 4*k);
        }
#else
        if (skip > 0)
        {
            memset(buf + 42 - 4*skip, ' ', 4*skip);
            memset(buf + 76 - 4*skip, ' ', 4*skip);
        }
        if (k < 8) 
        {
            memset(buf + 11, ' ', 32 - 4*k);
            memset(buf + 45, ' ', 32 - 4*k);
        }
#endif

        /* ---------------------------------------------------------------- */
        /*  Update the address field on the line.                           */
        /* ---------------------------------------------------------------- */
        buf[ 0] = hex[0xF & (addr >> 28)];
        buf[ 1] = hex[0xF & (addr >> 24)];
        buf[ 2] = hex[0xF & (addr >> 20)];
        buf[ 3] = hex[0xF & (addr >> 16)];
        buf[ 4] = hex[0xF & (addr >> 12)];
        buf[ 5] = hex[0xF & (addr >>  8)];
        buf[ 6] = hex[0xF & (addr >>  4)];
        buf[ 7] = hex[0xF & (addr      )];

        /* ---------------------------------------------------------------- */
        /*  Shuttle the line buffer to stdout for the user to see.          */
        /* ---------------------------------------------------------------- */
        fwrite(buf, 1, 77, stdout);
        addr += 8;

        skip = 0;
    }

    printf("\nResult failure ('%s' vs. '%s')\n", n1, n2);
    
    /* -------------------------------------------------------------------- */
    /*  Since we're running in "interactive debugging" mode, exit now       */
    /*  rather than continuing to compare the other arrays.                 */
    /* -------------------------------------------------------------------- */
    /*exit(1);*/

    return -1;
}

