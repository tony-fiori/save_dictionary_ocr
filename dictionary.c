#include <stdio.h>
#include <string.h>
#include <stdlib.h>

////////////////////// PARTIE LEVENSHTEIN ////////////////////////////

typedef enum {
    INSERTION,
    DELETION,
    SUBSTITUTION,
    NONE
} edit_type;
 
struct edit {
    unsigned int score;
    edit_type type;
    char arg1;
    char arg2;
    unsigned int pos;
    struct edit *prev;
};
typedef struct edit edit;
 
static int min3(int a, int b, int c)
{
    if (a < b && a < c) {
        return a;
    }
    if (b < a && b < c) {
        return b;
    }
    return c;
}
 
static unsigned int levenshtein_matrix_calculate(edit **mat, const char *str1, size_t len1, const char *str2, size_t len2)
{
    unsigned int i, j;
    for (j = 1; j <= len2; j++) {
        for (i = 1; i <= len1; i++) {
            unsigned int substitution_cost;
            unsigned int del = 0, ins = 0, subst = 0;
            unsigned int best;
            if (str1[i - 1] == str2[j - 1]) {
                substitution_cost = 0;
            }
            else {
                substitution_cost = 1;
            }
            del = mat[i - 1][j].score + 1; /* deletion */
            ins = mat[i][j - 1].score + 1; /* insertion */
            subst = mat[i - 1][j - 1].score + substitution_cost; /* substitution */
            best = min3(del, ins, subst);
            mat[i][j].score = best;                  
            mat[i][j].arg1 = str1[i - 1];
            mat[i][j].arg2 = str2[j - 1];
            mat[i][j].pos = i - 1;
            if (best == del) {
                mat[i][j].type = DELETION;
                mat[i][j].prev = &mat[i - 1][j];
            }
            else if (best == ins) {
                mat[i][j].type = INSERTION;
                mat[i][j].prev = &mat[i][j - 1];
            }
            else {
                if (substitution_cost > 0) {
                    mat[i][j].type = SUBSTITUTION;
                }
                else {
                    mat[i][j].type = NONE;
                }
                mat[i][j].prev = &mat[i - 1][j - 1];
            }
        }
    }
    return mat[len1][len2].score;
}
 
static edit **levenshtein_matrix_create(size_t len1, size_t len2)
{
    unsigned int i, j;
    edit **mat = malloc((len1 + 1) * sizeof(edit *));
    if (mat == NULL) {
        return NULL;
    }
    for (i = 0; i <= len1; i++) {
        mat[i] = malloc((len2 + 1) * sizeof(edit));
        if (mat[i] == NULL) {
            for (j = 0; j < i; j++) {
                free(mat[j]);
            }
            free(mat);
            return NULL;
        }
    }
    for (i = 0; i <= len1; i++) {
        mat[i][0].score = i;
        mat[i][0].prev = NULL;
        mat[i][0].arg1 = 0;
        mat[i][0].arg2 = 0;
    }
 
    for (j = 0; j <= len2; j++) {
        mat[0][j].score = j;
        mat[0][j].prev = NULL;
        mat[0][j].arg1 = 0;
        mat[0][j].arg2 = 0;
    }
    return mat; 
}


unsigned int levenshtein_distanc(const char *str1,const char *str2, edit **script)
{
    const size_t len1 = strlen(str1), len2 = strlen(str2);
    unsigned int i, distance;
    edit **mat, *head;
 
    /* If either string is empty, the distance is the other string's length */
    if (len1 == 0) {
        return len2;
    }
    if (len2 == 0) {
        return len1;
    }
    /* Initialise the matrix */
    mat = levenshtein_matrix_create(len1, len2);
    if (!mat) {
        *script = NULL;
        return 0;
    }
    /* Main algorithm */
    distance = levenshtein_matrix_calculate(mat, str1, len1, str2, len2);
    /* Read back the edit script */
    *script = malloc(distance * sizeof(edit));
    if (*script) {
        i = distance - 1;
        for (head = &mat[len1][len2];
                head->prev != NULL;
                head = head->prev) {
            if (head->type != NONE) {
                memcpy(*script + i, head, sizeof(edit));
                i--;
            }
        }
    }
    else {
        distance = 0;
    }
    /* Clean up */
    for (i = 0; i <= len1; i++) {
        free(mat[i]);
    }
    free(mat);
 
    return distance;
}

unsigned int levenshtein_distance(const char *str1, const char *str2)
{
    edit *script;
    unsigned int distance = levenshtein_distanc(str1, str2, &script);
    free(script);
    return distance;
}


void print(edit *e)
{
    if (e->type == INSERTION) {
        printf("Insert %c", e->arg2);
    }
    else if (e->type == DELETION) {
        printf("Delete %c", e->arg1);
    }
    else {
        printf("Substitute %c for %c", e->arg2, e->arg1);
    }
    printf(" at %u\n", e->pos);
}

//////////////////////// MAIN LEVENSHTEIN///////////////////////////////
/*
int main(){
    char* word1 = "zumstein";
    char* word2 = "sabotage";

    unsigned int distance = levenshtein_distance(word1, word2);
    printf("Levenshtein distance between \"%s\" and \"%s\" is : %u\n", word1, word2, distance);
}
*/

/////////////////////////// PARTIE OPENFILE /////////////////////////////////
int convert_str_int(char* str)
{
    int str_len = strlen(str);
    int r = 0;
    int i = 0;
    while (i < str_len)
    {
        r = 10 * r + (str[i] - '0');
        i++;
    }
    return r;
}


char* convert_length_filename(int length) // free malloc
{
    int nb_l = 1;
    if (length > 9){
        nb_l++;
    }
    char *str_nb = malloc (sizeof (char) * nb_l);
    for (int h = nb_l-1; h >= 0; h--)
    {
        str_nb[h] = (char)('0' + length % 10);
        length = length / 10;
    }
    str_nb[nb_l] = '\0';
    char * str_start = "dictionary_eng/length_";
    int l_start = strlen(str_start);
    char * str_ext = ".txt";
    int l_ext = strlen(str_ext);
    int length_filename = l_start + nb_l + l_ext; // "dictionary/length_" + [nb] + ".txt"
    char *filename = malloc (sizeof (char) * length_filename);
    strcpy(filename, str_start);
    strcat(filename, str_nb);
    free(str_nb);
    strcat(filename, str_ext);
    
    return filename;
}

char* convert_filenameocr_filenamedupli(const char* filenameocr) // free malloc
{
    char *start_dupli = ".";
    char *twotxt = "2.txt";
    int length_dupli = strlen(start_dupli) + strlen(twotxt) + strlen(filenameocr);
    char* filename_dupli = malloc(sizeof(char)* length_dupli);
    strcpy(filename_dupli, start_dupli);
    strcat(filename_dupli,filenameocr);
    strcat(filename_dupli,twotxt);
    return filename_dupli;
}

char* convert_filenameocr_filenameref(const char* filenameocr) // free malloc
{
    char *start_ref = ".ref_";
    char *ext = ".txt";
    int length_ref = strlen(start_ref) + strlen(ext) + strlen(filenameocr);
    char* filename_ref = malloc(sizeof(char)* length_ref);
    strcpy(filename_ref, start_ref);
    strcat(filename_ref,filenameocr);
    strcat(filename_ref,ext);
    return filename_ref;
}

char *next_word(FILE* file, int length, int *finished) // free malloc
{
    char* word = malloc(sizeof (char) * length);
    int i = 0;
    int int_charr = fgetc(file);
    if (int_charr == EOF)
        *finished = 1;
    while (int_charr != EOF && int_charr != '\n' && i<length)
    {
        word[i] = int_charr;
        int_charr = fgetc(file);
        i++;
    }
    word[i] = '\0';
    if (int_charr == EOF){
        *finished = 1;
    }
    //fgetc(file);
    return word;
}

int compare(char* ocr_word, char* file_word)
{
    int r = 1;
    int i = 0;
    while (r == 1 && ocr_word[i] != '\0' && file_word[i] != '\0'){
        if (ocr_word[i] != file_word[i]){
            r = 0;
        }
        i++;
    }
    return r;
}

int exist_eng(char* ocr_word)
{
    int l_word = strlen(ocr_word);
    if (l_word < 3)
        return 0;
    char* filename = convert_length_filename(l_word);
    FILE* file = fopen(filename,"r");

    int finish = 0;
    int *pf = &finish;


    while (*pf != 1){
        char* word_file = next_word(file, l_word, pf);
        char end = fgetc(file);
        if (end == EOF)
            *pf = 1;
        unsigned int distance = levenshtein_distance(word_file, ocr_word);
        free(word_file);
        if (distance == 0)
            return 1;
    }
    fclose(file);
    free(filename);
    return 2;
}

int __save_correction(char* ocr_word, int nb)
{
    int l_word = strlen(ocr_word);
    if (l_word < 3)
        return 0;
    printf("l_word : %i\n",l_word);
    char* filename = convert_length_filename(l_word);
    printf("filename : %s\n", filename);
    FILE* file = fopen(filename,"r");

    int finish = 0;
    int *pf = &finish;

    unsigned int min_dist = 50;
    char sk_word[l_word];

    int nbb = nb;

    while (*pf != 1){
        char* word_file = next_word(file, l_word, pf);
        char end = fgetc(file);
        if (end == EOF)
            *pf = 1;
        unsigned int distance = levenshtein_distance(word_file, ocr_word);
        printf("%s, %u\n", word_file, distance);
        if (distance == min_dist && nbb>0){
            nbb--;
            strcpy(sk_word,word_file);
        }
        if (distance < min_dist){
            min_dist = distance;
            //sk_word = word_file;
            //printf("check5\n");
            nbb = nb;
            strcpy(sk_word, word_file);
        }

        free(word_file);
    }
    printf("check1\n");
    //strcpy(correction, sk_word);
    /*
    int j = 0;
    while (sk_word[j] != '\n'){
        correction[j]  = sk_word[j];
        j++;
    }
    correction[j]='\0';*/
    //correction = sk_word;
    printf("check2\n");
    printf("\nsk_word : %s\n",sk_word);
    printf("min_dist : %u\n", min_dist);

    fclose(file);
    free(filename);
    //free(word_file);
    int r = compare(ocr_word, sk_word);
    return r;
}

char* correction(char* ocr_word, int nb, int plus) // free malloc
{
    int l_word = strlen(ocr_word);
    if (l_word < 3){
        char *r = malloc(sizeof(char) * l_word);
        strcpy(r, ocr_word);
        return r;
    }

    l_word +=plus;
    char* filename = convert_length_filename(l_word);
    FILE* file = fopen(filename,"r");

    int finish = 0;
    int *pf = &finish;

    unsigned int min_dist = 50;
    char sk_word[l_word];

    int nbb = nb;

    while (*pf != 1){
        char* word_file = next_word(file, l_word, pf);
        char end = fgetc(file);
        if (end == EOF)
            *pf = 1;
        unsigned int distance = levenshtein_distance(word_file, ocr_word);
        //printf("%s, %u\n", word_file, distance);
        if (distance == min_dist && nbb>0){
            nbb--;
            strcpy(sk_word,word_file);
        }
        if (distance < min_dist){
            min_dist = distance;
            //sk_word = word_file;
            //printf("check5\n");
            nbb = nb;
            strcpy(sk_word, word_file);
        }

        free(word_file);
    }
    char *r = malloc(sizeof(char)*l_word);
    strcpy(r, sk_word);
    //printf("\nsk_word : %s\n", r);
    //printf("min_dist : %u\n", min_dist);

    fclose(file);
    free(filename);

    return r;
}

int count_word(char *filename)
{
    FILE *file = fopen(filename,"r");
    int finish = 0;
    int *pf = &finish;

    int in_word = 0;
    int count = 0;

    char charr = fgetc(file);
    if (charr == EOF)
        return 0;
    while (*pf != 1)
    {
        if ((('a' <= charr && charr <= 'z') || ('A' <= charr && charr <= 'Z')) && in_word == 0){
            in_word = 1;
            count++;
        }
        else{
            if (!('a' <= charr && charr <= 'z') && !('A' <= charr && charr <= 'Z') && in_word == 1){
                in_word = 0;
            }
        }

        charr = fgetc(file);
        if (charr == EOF)
            *pf = 1;
    }

    fclose(file);

    return count;
}

void initialise(char *filename)
{
    printf("check1\n");
    FILE* ocr = fopen(filename, "r");
    char *filename_dupli = ".";
    char *filename_ref = ".ref_";
    char *twotxt = "2.txt";
    printf("check2\n");
    strcat(filename_dupli,filename);
    printf("check3\n");
    strcat(filename_dupli,twotxt);
    printf("check4\n");
    strcat(filename_ref,filename);
    printf("check5\n");
    strcat(filename_ref,twotxt);

    printf("check6\n");
    FILE* dupli = fopen(filename_dupli,"a");
    FILE* ref = fopen(filename_ref,"a");

    char charr = fgetc(ocr);
    char *str = "";
    while (charr != EOF){
        char *temp = malloc(sizeof(char) * 1);
        temp[0] = charr;
        fputs(temp, dupli);
        free(temp);
        if ((('a' <= charr && charr <= 'z') || ('A' <= charr && charr <= 'Z'))){
            char *temp = malloc(sizeof(char) * 1);
            temp[0] = charr;
            strcat(str, temp);
            free(temp);
        }
        else{
            if (strcmp("", str) != 0){
                char *temp = malloc(sizeof(char)*2);
                int index = exist_eng(str);
                temp[0] = '0' + index/10;
                temp[1] = '0' + index%10;
                fputs(temp,ref);
                free(temp);
                fputs(" ",ref);
                str = "";
            }
        }
        
        charr = fgetc(ocr);
    }

    if (strcmp("", str) != 0){
        char *temp = malloc(sizeof(char)*2);
        int index = exist_eng(str);
        temp[0] = '0' + index/10;
        temp[1] = '0' + index%10;
        fputs(temp,ref);
        free(temp);
        fputs(" ",ref);
    }


    // nav OCR file
    fclose(ocr);
}

int nb_solutions(char* word, int plus)
{
    int l_word = strlen(word);
    if (l_word < 3){
        return 0;
    }

    l_word +=plus;
    char* filename = convert_length_filename(l_word);
    FILE* file = fopen(filename,"r");

    int finish = 0;
    int *pf = &finish;

    int nb = 0;

    unsigned int min_dist = 50;
    char sk_word[l_word];

    while (*pf != 1){
        char* word_file = next_word(file, l_word, pf);
        char end = fgetc(file);
        if (end == EOF)
            *pf = 1;
        unsigned int distance = levenshtein_distance(word_file, word);
        //printf("%s, %u\n", word_file, distance);
        if (distance == min_dist){
            nb++;
            strcpy(sk_word,word_file);
        }
        if (distance < min_dist){
            min_dist = distance;
            //sk_word = word_file;
            //printf("check5\n");
            nb = 1;
            strcpy(sk_word, word_file);
        }

        free(word_file);
    }

    fclose(file);
    free(filename);

    return nb;
}

void correction_solutions(char* word, int var_avant, int var_apres)
{
    int r_exist = exist_eng(word);

    if (r_exist == 0)
    {
        printf("\"%s\" not long enough for correction !\n", word);
    }
    if (r_exist == 1)
    {
        printf("\"%s\" is correct.\n", word);
    }
    if (r_exist == 2)
    {
        printf("Possible solutions :\n");
        int i = 0;
        int j = var_avant;
        int nb = nb_solutions(word, j);
        while (j < var_apres +1){//2
            i = 0;
            while (i < nb){
                char* r = correction(word , i, j);
                printf("%s\n", r);
                free(r);
                i++;
            }
            j++;
            nb = nb_solutions(word, j);
        }
    }
}

char* first_solution(char* word)
{
    int length = strlen(word);
    if (length >= 1 && 'A'<= word[0] && word[0] <= 'Z'){
        word[0] += 'a' - 'A';
    }
    char *r = correction(word,0,0);
    return r;
}


void first_file(char* filename){
    int length_max = 50;
    int l_filename = strlen(filename);
    char* start = "c_";
    int l_start = strlen(start);
    int l_filename_dupli = l_start + l_filename;
    char* filename_dupli = malloc(sizeof(char) * l_filename_dupli);
    strcpy(filename_dupli, start);
    strcat(filename_dupli, filename);
    filename_dupli[l_filename_dupli] = '\0';
    FILE* file = fopen(filename,"r");
    FILE* file_dupli = fopen(filename_dupli,"a");

    int first_maj = 0;
    int index = 0;
    
    int int_charr = fgetc(file);
    if ('A' <= int_charr && int_charr <= 'Z'){
        int_charr += 'a' - 'A';
        first_maj = 1;
    }
    while (int_charr != EOF)
    {
        while (int_charr != EOF && !('A' <= int_charr && int_charr <= 'Z')&& !('a' <= int_charr && int_charr <= 'z'))
        {
            // insertion fichier int_charr
            char* one_char = malloc(sizeof(char)*1);
            one_char[0] = int_charr;
            one_char[1] = '\0';
            fputs(one_char, file_dupli);
            //printf("\'%c\' ",int_charr);
            free(one_char);
            int_charr = fgetc(file);
        }
        if (int_charr != EOF)
        {
            char* word = malloc(sizeof (char) * length_max);
            int i = 0;
            
            while (int_charr != EOF && (('A' <= int_charr && int_charr <= 'Z')|| ('a' <= int_charr && int_charr <= 'z')))
            {
                if ('A' <= int_charr && int_charr <= 'Z')
                {
                    if (i == 0)
                    {
                        first_maj = 1;
                    }
                    int_charr += 'a' - 'A';
                }
                word[i] = int_charr;
                int_charr = fgetc(file);
                i++;
            }
            word[i] = '\0';
            
            ////////////////////////////
            if (exist_eng(word) == 2)
            {
                char* t_word = first_solution(word);
                if (strlen(t_word) >= 1 && first_maj == 1)
                {
                    t_word[0] -= ('a' - 'A');
                }
                fputs(t_word, file_dupli);
                printf("mot[%i] = %s\n",index, t_word);
                free(t_word);
            }
            else
            {
                if (strlen(word) >= 1 && first_maj == 1)
                {
                    word[0] -= ('a' - 'A');
                }
                fputs(word, file_dupli);
                printf("mot[%i] = %s\n",index, word);
            }
            index++;
            free(word);
            while (int_charr != EOF && !('A' <= int_charr && int_charr <= 'Z')&& !('a' <= int_charr && int_charr <= 'z'))
            {
                // insertion fichier int_charr
                char* one_char = malloc(sizeof(char)*1);
                one_char[0] = int_charr;
                one_char[1] = '\0';
                fputs(one_char, file_dupli);
                //printf("\'%c\' ",int_charr);
                free(one_char);
                int_charr = fgetc(file);
            }
        }
        first_maj = 0;
    }
    free(filename_dupli);
    fclose(file);
    fclose(file_dupli);
}

/*
void first_solution_on_file(char* filename){
    char* fs_filename = malloc(sizeof(char)* )
}*/

int transform_str_int(char* str)
{
    int str_len = strlen(str);
    int index = 0;
    int negative = 0;
    int r = 0;
    if (str_len >= 1 && str[0] == '-'){
        negative = 1;
        index++;
    }
    while (index < str_len)
    {
        r = 10 * r + (str[index] - '0');
        index++;
    }

    if (negative == 1){
        r*= -1;
    }

    return r;
}

/*
void modification(char* filenameref, char* filename_dupli, int index_word, int mod)
{
    int length_max = 50;
    int l_filename = strlen(filename);
    char* start = ".c_";
    int l_start = strlen(start);
    int l_filename_dupli = l_start + l_filename;
    char* filename_dupli = malloc(sizeof(char) * l_filename_dupli);
    strcpy(filename_dupli, start);
    strcat(filename_dupli, filename);
    filename_dupli[l_filename_dupli] = '\0';
    FILE* file = fopen(filename,"r");
    FILE* file_dupli = fopen(filename_dupli,"a");

    int first_maj = 0;
    int index = 0;
    
    int int_charr = fgetc(file);
    if ('A' <= int_charr && int_charr <= 'Z'){
        int_charr += 'a' - 'A';
        first_maj = 1;
    }
    while (int_charr != EOF)
    {
        while (int_charr != EOF && !('A' <= int_charr && int_charr <= 'Z')&& !('a' <= int_charr && int_charr <= 'z'))
        {
            // insertion fichier int_charr
            char* one_char = malloc(sizeof(char)*1);
            one_char[0] = int_charr;
            one_char[1] = '\0';
            fputs(one_char, file_dupli);
            //printf("\'%c\' ",int_charr);
            free(one_char);
            int_charr = fgetc(file);
        }
        if (int_charr != EOF)
        {
            char* word = malloc(sizeof (char) * length_max);
            int i = 0;
            
            while (int_charr != EOF && (('A' <= int_charr && int_charr <= 'Z')|| ('a' <= int_charr && int_charr <= 'z')))
            {
                if ('A' <= int_charr && int_charr <= 'Z')
                {
                    if (i == 0)
                    {
                        first_maj = 1;
                    }
                    int_charr += 'a' - 'A';
                }
                word[i] = int_charr;
                int_charr = fgetc(file);
                i++;
            }
            word[i] = '\0';
            

            if (i == index_word)
            {
                int ind_s = 0:
                char* test = correction()
            }
            ////////////////////////////
            if (exist_eng(word) == 2)
            {
                char* t_word = first_solution(word);
                if (strlen(t_word) >= 1 && first_maj == 1)
                {
                    t_word[0] -= ('a' - 'A');
                }
                fputs(t_word, file_dupli);
                printf("mot[%i] = %s\n",index, t_word);
                free(t_word);
            }
            else
            {
                if (strlen(word) >= 1 && first_maj == 1)
                {
                    word[0] -= ('a' - 'A');
                }
                fputs(word, file_dupli);
                printf("mot[%i] = %s\n",index, word);
            }
            //////////////////////////
            index++;
            free(word);
            while (int_charr != EOF && !('A' <= int_charr && int_charr <= 'Z')&& !('a' <= int_charr && int_charr <= 'z'))
            {
                // insertion fichier int_charr
                char* one_char = malloc(sizeof(char)*1);
                one_char[0] = int_charr;
                one_char[1] = '\0';
                fputs(one_char, file_dupli);
                //printf("\'%c\' ",int_charr);
                free(one_char);
                int_charr = fgetc(file);
            }
        }
        first_maj = 0;
    }
    free(filename_dupli);
    fclose(file);
    fclose(file_dupli);
}
*/

void put_in_file(char* str, char* filename){
    FILE* file = fopen(filename,"w");
    fputs(str, file);
    fclose(file);
}

char* from_file(char* filename){ // don t forget to free() the string after usage
    FILE* file = fopen(filename,"r");
    char charr = fgetc(file);
    int len = 0;
    while (charr != EOF){
        len++;
        charr = fgetc(file);
    }
    fclose(file);
    FILE* file2 = fopen(filename, "r");
    char* str = malloc(sizeof(char) * len);
    char charrr = fgetc(file2);
    int i = 0;
    while(i < len && charrr != EOF){
        str[i] = charrr;
        i++;
        charrr = fgetc(file2);
    }
    fclose(file2);
    str[i] = '\0';
    return str;
}


///////////////////////////// MAIN OPENFILE /////////////////////////////////
/*
int main(int argc, char* argv[]){

    //correction_solutions(argv[1], 0, 1);
    int first = 1;
    int done = 0;
    if (argc == 3 && strcmp("file" , argv[1]) == 0){
        first = 0;
        done = 1;
        first_file(argv[2]);
    }
    
    
    int display_argv = 0; // FALSE
    
    if (argc == 2){
        //char* r = correction(argv[1], 0, 0);
        //printf("correction : %s\n", r);
        //free(r);
        int argv2 = 0;
        int argv3 = 0;
        if (display_argv == 1){
            printf("argv2 = %i\n",argv2);
            printf("argv3 = %i\n", argv3);
        }
        correction_solutions(argv[1], argv2, argv3);
        done = 1;
    } 
    if (first == 1 && argc == 3){
        int argv2 = transform_str_int(argv[2]);
        int argv3 = 0;
        if (display_argv == 1){
            printf("argv2 = %i\n",argv2);
            printf("argv3 = %i\n", argv3);
        }
        //char* r = correction(argv[1], argv2, 0);
        //printf("correction : %s\n", r);
        //free(r);
        correction_solutions(argv[1],argv2,argv3);
        done = 1;
    }
    if (argc == 4){
        int argv2 = transform_str_int(argv[2]);
        int argv3 = transform_str_int(argv[3]);
        if (display_argv == 1){
            printf("argv2 = %i\n",argv2);
            printf("argv3 = %i\n", argv3);
        }
        //char* r = correction(argv[1], argv2, argv3);
        //printf("correction : %s\n", r);
        //free(r);
        correction_solutions(argv[1],argv2, argv3);
        done = 1;
    }
    if (done == 0){
        printf("Usage : ./a.out [word] [length_min] [length_max]\n");
    }

    return 0;
}*/

int main(){
    char* filename = "ocr_text.txt";
    char* filename_correction = "c_ocr_text.txt"; // fichier caché 
    char* str_ocr = "This is my girst correctjon!";

    put_in_file(str_ocr, filename);

    first_file(filename);

    char* after_first_correction = from_file(filename_correction);
    printf("%s\n",after_first_correction);
    free(after_first_correction);
    
    return 0; 
}


/*
    //printf("%d\n",count_word(argv[1]));
    printf("check0\n");

    //initialise(argv[1]);
    printf("1\n");
    char* filename_ref = convert_filenameocr_filenameref(argv[1]);
    printf("2\n");
    char* filename_dupli = convert_filenameocr_filenamedupli(argv[1]);

    printf("ref : %s\ndupli : %s\n",filename_ref, filename_dupli);


    return argc;*/
    
    
    /*if (argc == 1)
        return 1;
    if (argc == 2)
        printf("result exist : %i\n",exist_eng(argv[1]));*/
    
    /*
    if (argc == 1)
        return 1;
    if (argc == 2){
        printf("result exist : %i\n",exist_eng(argv[1],0));
    }
        
    if (argc == 3){
        int nb = 2;
        printf("result exist : %i\n",exist_eng(argv[1],nb));
        printf("nb = %i\n", nb);
    }*/
    /*
    char* word = "atmosphere";
    int l_word = strlen(word);

    char* filename = convert_length_filename(l_word);
    printf("filename = %s\n", filename);

    FILE* file = fopen(filename,"r");
    printf("check1\n");

    int finished = 0;
    unsigned int distance = 0;
    int *pf = &finished;
    char* word_file;
    char* sk_word = "zzzzzzzzzzzzz";
    unsigned int min_dist = 50;
    //word_file = next_word(file, l_word, pf);
    //printf("check2 %s \n",word_file);
    while (*pf != 1){
        word_file = next_word(file, l_word, pf);
        printf("%s",word_file);
        distance = levenshtein_distance(word_file, word);
        if (distance < min_dist){
            min_dist = distance;
            sk_word = word_file;
        }
        printf(" distance = %u\n", distance );
    }
    printf("MAIN FINISHED\n");
    printf("minimun distance %u => %s\n", min_dist, sk_word);
    printf("compare result : %i\n",compare(word, sk_word));
    //
    fclose(file);
    free(filename);
    free(word_file);
    //*/