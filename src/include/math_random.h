void random_seed(void);

int random_range(int lower, int upper);
int random_range_uniform(int rangeLow, int rangeHigh);

double random_zero_one(void);
double random_zero_one_mt19937(void);
double random_pm_one(void);
double random_pm_one_mt19937(void);

#define RAND_MT19937ar
#define RAND_MT19937ar_cok

#ifdef RAND_MT19937ar
    /* initializes mt[N] with a seed */
    void random_mt19937ar_init_genrand(unsigned long s);

    /* initialize by an array with array-length */
    /* init_key is the array for initializing keys */
    /* key_length is its length */
    /* slight change for C++, 2004/2/26 */
    void random_mt19937ar_init_by_array(unsigned long init_key[], int key_length);
    
    /* generates a random number on [0,0xffffffff]-interval */
    unsigned long random_mt19937ar_int32(void);

    /* generates a random number on [0,0x7fffffff]-interval */
    long random_mt19937ar_int31(void);

    /* These real versions are due to Isaku Wada, 2002/01/09 added */
    /* generates a random number on [0,1]-real-interval */
    double random_mt19937ar_real1(void);

    /* generates a random number on [0,1)-real-interval */
    double random_mt19937ar_real2(void);

    /* generates a random number on (0,1)-real-interval */
    double random_mt19937ar_real3(void);

    /* generates a random number on [0,1) with 53-bit resolution*/
    double random_mt19937ar_res53(void);
#endif

#ifdef RAND_MT19937ar_cok
    /* initializes mt[N] with a seed */
    void random_mt19937ar_cok_init_genrand(unsigned long s);

    /* initialize by an array with array-length */
    /* init_key is the array for initializing keys */
    /* key_length is its length */
    /* slight change for C++, 2004/2/26 */
    void random_mt19937ar_cok_init_by_array(unsigned long init_key[], int key_length);
    
    void random_mt19937ar_cok_next_state(void);

    /* generates a random number on [0,0xffffffff]-interval */
    unsigned long random_mt19937ar_cok_int32(void);

    /* generates a random number on [0,0x7fffffff]-interval */
    long random_mt19937ar_cok_int31(void);

    /* These real versions are due to Isaku Wada, 2002/01/09 added */
    /* generates a random number on [0,1]-real-interval */
    double random_mt19937ar_cok_real1(void);

    /* generates a random number on [0,1)-real-interval */
    double random_mt19937ar_cok_real2(void);

    /* generates a random number on (0,1)-real-interval */
    double random_mt19937ar_cok_real3(void);

    /* generates a random number on [0,1) with 53-bit resolution*/
    double random_mt19937ar_cok_res53(void);
#endif


