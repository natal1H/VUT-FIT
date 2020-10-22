/**
 * Kostra programu pro 3. projekt IZP 2017/18
 *
 * Jednoducha shlukova analyza
 * Unweighted pair-group average
 * https://is.muni.cz/th/172767/fi_b/5739129/web/web/usrov.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX
#include <string.h> // strcmp

#define MAX_LINE 100 // kvoli pomocnym retazcom
#define POSITION_MIN 0 // X,Y >= 0 - osetrenie suradnic
#define POSITION_MAX 1000 // X,Y <= 1000

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

int clustering_mode = 1; // metoda zhlukovania, 1 - unweighted pair-group average, 2 - najblizsi sused, 3 - najvzdialenejsi sused


/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/**
 * Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 * Ukazatel NULL u pole objektu znamena kapacitu 0.
 * \param *c    ukazovatel na zhluk, ktory chceme inicializovat
 * \param cap   kapacita, na ktoru chceme inicializovat zhluk
 */
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    // TODO
    if (cap < 0) // Chyba, nespravna kapacita
        return ;
    if (cap == 0) // prazdny zhluk
        c->obj = NULL;
    else 
        c->obj = malloc(cap*sizeof(struct obj_t));
    if (c->obj == NULL) 
        c->capacity = 0;
    else 
        c->capacity = cap;
    c->size = 0;
    
}

/**
 * Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 * \param *c    ukazovatel na zhluk, s ktorym pracujeme
 */
void clear_cluster(struct cluster_t *c)
{
    // TODO
    free(c->obj);
    init_cluster(c, 0);
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/**
 * Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 * \param *c      ukazovatel na zhluk, ktoreho kapacitu chceme zmenit
 * \param new_cap nova kapacita
 * \return        zhluk so zmenenou kapacitou
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;
    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/**
 * Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt nevejde.
 * \param *c    ukazovatel na zhluk, do ktoreho chceme pridat objekt
 * \param obj   objekt, ktory chceme pridat do zhluku 
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    // TODO
    if (c->size >= c->capacity) // treba zmenit kapacitu
        c = resize_cluster(c, c->capacity + CLUSTER_CHUNK); 
    if (c == NULL) // chyba
        return ;  
    c->obj[c->size] = obj;
    c->size += 1;
}

void sort_cluster(struct cluster_t *c);

/**
 * Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 * Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 * Shluk 'c2' bude nezmenen.
 * \param *c1    ukazovatel na prvy zhluk
 * \param *c2    ukazovatel na druhy zhluk
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    // TODO
    for (int i = 0; i < c2->size; i++) // pridanie objektov z c2
        append_cluster(c1, c2->obj[i]);
    sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

/**
 * Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 * (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 * pocet shluku v poli.
 * \param *carr    ukazovatel na pole zhlukov
 * \param narr     velkost pola zhlukov
 * \param idx      index zhluku, ktory chceme odstranit
 * \return         novy pocet zhlukov v poli
 */
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    // TODO
    clear_cluster(&carr[idx]);
    for (int i = idx; i < narr-1; i++) // presunutie zhlukov od idx
        carr[i] = carr[i+1];
    return narr-1;
}

/**
 * Pocita Euklidovskou vzdalenost mezi dvema objekty.
 * \param *o1    ukazovatel na prvy objekt   
 * \param *o2    ukazovatel na druhy objekt
 * \return       vzdialenost medzi objektami
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    // TODO
    return sqrtf( (o1->x - o2->x)*(o1->x - o2->x) + (o1->y - o2->y)*(o1->y - o2->y) );
}

/**
 * Pocita vzdalenost dvou shluku.
 * \param *c1    ukazovatel na prvy zhluk
 * \param *c2    ukazovatel na druhy zhluk
 * \return       vzdialenost medzi zhlukmi
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    // TODO
    if (clustering_mode == 1) {
        float total_dist = 0;
        for (int i = 0; i < c1->size; i++) { // postupne beriem vsetky objekty z c1
            for (int j = 0; j < c2->size; j++)  // ku kazdemu objektu z c1 beriem postupne vsetky objekty z c2
                total_dist += obj_distance(&(c1->obj[i]), &(c2->obj[j]));
        }
        return total_dist / c1->size / c2->size; // aby sa spriemerovala vzdialenost, musim este predelit velkostami clusterov
    }
    else if (clustering_mode == 2) {
        float min_dist = INFINITY;
        for (int i = 0; i < c1->size; i++) { // postupne beriem vsetky objekty z c1
            for (int j = 0; j < c2->size; j++) {  // ku kazdemu objektu z c1 beriem postupne vsetky objekty z c2
                float dist = obj_distance(&(c1->obj[i]), &(c2->obj[j]));
                if (dist < min_dist)
                    min_dist = dist;
            }
        }
        return min_dist;
    }
    else {
        float max_dist = -1;
        for (int i = 0; i < c1->size; i++) { // postupne beriem vsetky objekty z c1
            for (int j = 0; j < c2->size; j++) { // ku kazdemu objektu z c1 beriem postupne vsetky objekty z c2
                float dist = obj_distance(&(c1->obj[i]), &(c2->obj[j]));
                if (dist > max_dist)
                    max_dist = dist;
            }
        }
        return max_dist;
    }
}

/**
 *  Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 *  hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 *  'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 *  adresu 'c1' resp. 'c2'.
 *  \param *carr    pole zhlukov
 *  \param narr     velkost pola zhlukov
 *  \param *c1      ukazovatel na index prveho z dvojice najblizsich zhlukov
 *  \param *c2      ukazovatel na index druheho z dvojice najblizsich zhlukov
 */
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    // TODO
    float min_dist = INFINITY;

    for (int i = 0; i < narr; i++) {
        for (int j = i + 1; j < narr; j++) {
            float dist = cluster_distance(&carr[i], &carr[j]);
            if (dist < min_dist) { // najdenie novej mensej vzdialenosti
                *c1 = i;
                *c2 = j;
                min_dist = dist;
            }
        }
    }
}


// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/**
 * Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
 * \param *c    ukazovatel na zhluk na usporiadanie 
 */
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/**
 * Tisk shluku 'c' na stdout.
 * \param *c    ukazovatel na zhluk na vypisanie
 */
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/**
 * Nacitanie double zo str a kontrola spravnosti prevodu
 * \param  *p_number    ukazovatel na int, kde chceme ulozit cislo
 * \param  str[]        retazec, ktory chceme previest na int
 * \return              chybovy kod 
 */
int readInt(int *p_number, char str[]) {
    char *end_string;
    *p_number = strtol(str, &end_string, 10);
    if (end_string[0] != '\0') // kontrola ci cislo neobsahovalo nepovolene znaky
        return EXIT_FAILURE;
    else
        return EXIT_SUCCESS;
}

/**
 * Kontrola rozsahu zadaneho float
 * \param  number    zadane cislo
 * \param  min       dolna hranica rozsahu (vratane)
 * \param  max       horna hranica rozsahu (vratane)  
 * \return           chybovy kod     
 */
int checkRangeInt(int number, int min, int max) {
    return (number >= min && number <= max) ? EXIT_SUCCESS : EXIT_FAILURE;
}

/**
 * Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 * jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 * polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 * kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 * V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
 * \param *filename    nazov suboru
 * \param **arr        ukazovatel na pole zhlukov
 * \return             pocet nacitanych zhlukov
 */
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

    // TODO
    int loaded_clusters = 0;
    FILE *f = fopen(filename, "r"); 
    if (f == NULL) { // nastala chyba pri otvoreni suboru
        fprintf(stderr, "Chyba pri otvarani suboru.\n");
        *arr = NULL;
        return loaded_clusters;
    }

    int num_clusters; // pocet zhlukov na nacitanie
    char str_num[MAX_LINE] = "";
    fscanf(f, "count=%s", str_num); // nacitanie poctu objektov do retazca kvoli naslednej kontrole spravnosti

    // Osetrenie chyb
    if (str_num[0] == '\0' || readInt(&num_clusters, str_num) == EXIT_FAILURE) { // Zly format alebo chyba pri prevode
        fprintf(stderr, "Chyba, nespravny format.\n");
        *arr = NULL;
        return loaded_clusters;
    }

    // alokacia miesta pre pole 'num_clusters' zhlukov
    *arr = malloc(num_clusters * sizeof(struct cluster_t)); 
    if (arr == NULL)  // chyba pri alokacii
        return loaded_clusters;
  
    for (int i = 0; i < num_clusters; i++) // alokacia jednotlivych zhlukov
        init_cluster(&(*arr)[i], 1);

    while (loaded_clusters < num_clusters) {
        int id;
        int x, y;

        char str_id[MAX_LINE] = "", str_x[MAX_LINE] = "", str_y[MAX_LINE] = ""; 

        if (fscanf(f, "%s %s %s", str_id, str_x, str_y) == EOF) // Doslo na koniec suboru
            break;

        if (str_id[0] == '\0' || str_x[0] == '\0' || str_y[0] == '\0') { // Chyba nejaky udaj
            fprintf(stderr, "Chyba, chybajuci udaj.\n");
            *arr = NULL;
            return loaded_clusters;
        }
	else if ( readInt(&id, str_id) == EXIT_FAILURE || readInt(&x, str_x) == EXIT_FAILURE || readInt(&y, str_y) == EXIT_FAILURE) { // Chyba pri prevode cisla
            fprintf(stderr, "Chyba pri prevode cisla.\n");
            *arr = NULL;
            return loaded_clusters;
        }
        else if ( checkRangeInt(x, POSITION_MIN, POSITION_MAX) == EXIT_FAILURE || checkRangeInt(y, POSITION_MIN, POSITION_MAX) ) { // Cislo mimo rozsah
            fprintf(stderr, "Chyba v rozsahu cisiel.\n");
            *arr = NULL;
            return loaded_clusters;
        }
        // Vsetky potrebne udaje spravne nacitane
        struct obj_t tmp_obj = {id,(float) x, (float) y};
        append_cluster(&(*arr)[loaded_clusters], tmp_obj); // pridanie objektu
        loaded_clusters++;
    }

    // Osetrenie chyb
    if (fclose(f) == EOF) { //nastala chyba pri uzavreti suboru, dopln chybovy vypis
        fprintf(stderr, "Chyba pri zatvarani suboru.\n");
        *arr = NULL;
    }  
    else if (loaded_clusters < num_clusters) { // Menej objektov v subori ako zadany pocet
        fprintf(stderr, "Chyba pri nacitani objektov zo suboru.\n");
        *arr = NULL;
    }

    return loaded_clusters;
}

/**
 * Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 * Tiskne se prvnich 'narr' shluku.
 * \param *carr    ukazovatel na prvy zhluk
 * \param narr     velkost pola zhlukov
 */
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[])
{
    struct cluster_t *clusters;

    // TODO
    if (argc < 2 || argc > 4) { // Kontrola poctu argumentov
        fprintf(stderr, "Chyba, nespravny pocet argumentov.\n");
        return EXIT_FAILURE;
    }
    int num_clusters = 1;
    if (argc >= 3) { // Kontrola pritomnosti volitelneho argumentu N
        if ( readInt(&num_clusters, argv[2]) == EXIT_FAILURE || num_clusters < 1) { // Kontrola spravnosti nacitania N
            fprintf(stderr, "Chyba, nespravne zadany pocet pozadovanych zhlukov.\n");
            return EXIT_FAILURE;
        }
    }
    
    if (argc == 4) { // volitelny argument metoda zhlukovania
        if (strcmp(argv[3], "--avg") == 0)
            clustering_mode = 1;
        else if (strcmp(argv[3], "--min") == 0)
            clustering_mode = 2;
        else if (strcmp(argv[3], "--max") == 0)
            clustering_mode = 3;
        else {
            fprintf(stderr, "Chyba, nespravny argument.\n");
            return EXIT_FAILURE;
        }
    }
    int num_loaded = load_clusters(argv[1], &clusters);

    // Osetrenie chyb
    if (clusters == NULL) // Nastala chyba vo funkcii load_clusters
        return EXIT_FAILURE;
    else if (num_loaded < num_clusters) { // Pocet nacitanych objektov mensi ako pozadovany pocet zhlukov
        fprintf(stderr, "Chyba, menej nacitanych objektov ako pozadovanych zhlukov.\n");
        return EXIT_FAILURE;
    }

    // Spajanie zhlukov kym nedosiahneme pozadovany pocet zhlukov
    for (int i = num_loaded; i > num_clusters; i--) {
        int idx1, idx2; // indexy dvoch najblizsich zhlukov
        find_neighbours(clusters, i, &idx1, &idx2);
        merge_clusters(&(clusters)[idx1], &(clusters)[idx2]); // spojenie dvoch najblizsich zhlukov
        remove_cluster(clusters, i, idx2); // odstranenie druheho zhluku z dvojice najblizsich zhlukov z pola zhlukov
    }

    print_clusters(clusters, num_clusters);
    
    // Postupne uvolenie pamate alokovej pre pole zhlukov
    for (int i = 0; i < num_clusters; i++) {
        clear_cluster(&clusters[i]);
    }
    free(clusters);

    return EXIT_SUCCESS; // Uspech
}