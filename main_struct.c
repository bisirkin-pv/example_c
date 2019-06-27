#include <stdio.h>       //подключение заголовочных файлов
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_LEN_TEXT 50  //длина тектовых полей структуры
#define MENU_LINE "-----------------------------------------------------"
#define INVITE_STR " > " //строка приглаения ввода
#define FILENAME "movies.db"

int str_len(char *s) { //возвращение длины строки
    
    int n;
    
    for (n = 0; *s != '\0'; s++) n++;
    
    return n;
}


int read_line(char ch[], char *caption) { //считывание пользовательского ввода (в тексте разрешены пробелы)
    
    printf(" введите %s: ", caption);
    scanf("%[^\n]%*c", ch);
    
    return 0;
}


int read_int(char *caption) { //считывание числа
    
    int number;
    
    printf(" введите %s: ", caption);
    scanf("%d", &number);
    
    return number;
}

int check_answer(int answer, int max_value) { //проверка допустимого ввода номера меню
    
    if (answer <= 0 || answer > max_value) {
        if (answer == 0) {
            printf(" завершение программы\n");
            exit(0);
        }
        printf(" error: некорректный ввод\n");
        return 1;
    }
    return 0;
}

struct time { //структура времени
    int hh;                          //hh
    int mm;                          //mm
};

struct session { //структура киносеанса
    char name[MAX_LEN_TEXT];        //название
    char genre[MAX_LEN_TEXT];       //жанр
    int  number;                    //номер зала
    struct time time;              //hh:mm
};

int CNT = 0; //кол-во структур в массиве

struct session* write_stuct(int idx, struct session *movies) { //заполнение структуры
    char txt[MAX_LEN_TEXT];
    int number;
    getchar();
    printf("\n киносеанс #%d\n", idx + 1);
    read_line(txt, "название");
    strcpy((movies + idx)->name, txt);
    read_line(txt, "жанр");
    strcpy((movies + idx)->genre, txt);
    number = read_int("номер зала");
    (movies + idx)->number = number;
    number = read_int("время сеанса (hh)");
    (movies + idx)->time.hh = number;
    number = read_int("время сеанса (mm)");
    (movies + idx)->time.mm = number;
    
    return movies;
}

struct session* print_struct(int idx, struct session *movies) { //вывода структуры
    printf(MENU_LINE);
    printf("\n название фильма: %s\n", (movies + idx)->name);
    printf(" жанр: %s\n", (movies + idx)->genre);
    printf(" номер зала: %d\n", (movies + idx)->number);
    printf(" время сеанса: %02d:%02d\n", (movies + idx)->time.hh, (movies + idx)->time.mm);
    
    return movies;
}

int print_all_struct(struct session *movies) { //отображение полученного результата
    int idx;
    for (idx = 0; idx < CNT; idx++) print_struct(idx, movies);
    
    return 0;
}

struct session* open_struct_file(char *filename, char c[2]) { //открытие файла со структурой
    struct session* movies;
    FILE *file;
    
    int is_open;
    if ((movies = (struct session*)malloc(sizeof(struct session) * (1))) == NULL) {
        printf(" error: не удалось выделить пам€ть");
        exit(-1);
    }
    if ((file = fopen(filename, "rb+")) == NULL) {
        is_open = 0;
        printf(" нe удалось найти ранее созданный файл\n");
        return movies;
    }
    else {
        if (strncmp(c, "y", 1) == 0 || strncmp(c, "Y", 1) == 0) {
            int idx = 0;
            while (!feof(file)) {
                if (idx > 0) {
                    movies = realloc(movies, sizeof(struct session) * (idx + 1));
                }
                fread(&movies[idx], sizeof(struct session), 1, file);
                idx++;
            }
            CNT = --idx;
            printf(" файл успешно считан \n");
            is_open = 1;
        }
    }
    fclose(file);
    fflush(file);
    return movies;
}

int save_struct(char *filename, int len, struct session *movies) //сохранение структуры в файл
{
    FILE *file;
    if ((file = fopen(filename, "w")) == NULL) printf(" error: не удалось сохранить структуру в файл\n");
    else {
        for (int i = 0; i < len; i++) fwrite(&movies[i], sizeof(struct session), 1, file);
    }
    fclose(file);
    fflush(file);
    printf(" файл успешно сохранен \n");
    return 0;
}

int sort_struct(struct session *movies) { //сортировки по возрастанию кол-ва букв в названии
    struct session buf;
    int i, j;
    for (i = 0; i < CNT - 1; i++) {
        for (j = i + 1; j < CNT; j++) {
            if (str_len((movies + j)->name) < str_len((movies + i)->name)) {
                buf = movies[j];
                movies[j] = movies[i];
                movies[i] = buf;
            }
        }
    }
    return 0;
}

int add_struct(struct session *movies) { //добавление записи
    movies = realloc(movies, sizeof(struct session) * (CNT + 1));
    write_stuct(CNT++, movies);
    
    return 0;
}

int filter_struct(struct session *movies, int start_elem, char *genre, int hour_start, int hour_end, int minute_start, int minute_end) { //фильтра структуры по заданным критериям
    int i;
    for (i = start_elem + 1; i < CNT; i++) {
        size_t ln_genre = sizeof(genre);
        if (
            (strncmp((movies + i)->genre, genre, ln_genre) == 0)
            && ((movies + i)->time.hh >= hour_start && (movies + i)->time.mm >= minute_start)
            && ((movies + i)->time.hh < hour_end || ((movies + i)->time.hh = hour_end && (movies + i)->time.mm <= minute_end))
            )
            return i;
    }
    printf(" указанных записей не найдено\n");
    return -1;
}

int find_struct(struct session *movies, char *genre, int hour_start, int hour_end, int minute_start, int minute_end) { //поиск структуры
    int FILTER_POSITION = -1;
    
    while ((FILTER_POSITION = filter_struct(movies, FILTER_POSITION, genre, hour_start, hour_end, minute_start, minute_end)) >= 0) {
        print_struct(FILTER_POSITION, movies);
    }
    
    return FILTER_POSITION;
}

int delete_struct(int idx, struct session *movies) { //удаление заданной структуры
    int i;
    
    
    if (idx < 0 || idx > CNT) {
        printf(" элемента с номером #%d не существует\n", idx);
        return 1;
    }
    
    for (i = idx - 1; i < CNT - 1; i++) {
        movies[i] = movies[i + 1];
    }
    movies = realloc(movies, sizeof(struct session) * (CNT - 1));
    CNT--;
    printf(" элемент #%d удален\n", idx);
    
    return 0;
}

int main() {
    
    char genre[MAX_LEN_TEXT];
    int hour_start, hour_end, minute_start, minute_end, idx;
    int answer = 0;
    int repeat = 1;
    int is_change = 1;
    char c[2];
    
    system("chcp 1251");
    
    struct session *movies;
    movies = (struct session*)malloc(sizeof(struct session) * (1));
    if (movies == NULL) {
        printf(" error: не удалось выделить пам€ть");
        //exit(-1);
    }
    
    while (repeat == 1) {
        printf("%s\n программа формировани€ данных о киносеансах \n%s\n", MENU_LINE, MENU_LINE);
        printf("%s\n главное меню \n%s\n", MENU_LINE, MENU_LINE);
        printf(" выбирете требуемое действие:\n");
        printf("  [0] выход из любого меню\n");
        printf("  [1] чтение файла\n");
        printf("  [2] новый файл\n");
        printf(INVITE_STR);
        scanf("%d", &answer);
        repeat = check_answer(answer, 3);
    }
    
    repeat = 1;
    
    if (answer == 1) {
        printf(" найден ранее созданный файл, открыть его? [y,n] : ");
        scanf("%s", c);
        movies = open_struct_file(FILENAME, c);   //попытки нахождени€ файла
        if (CNT == 0) {
            printf("  [0] выйти из меню\n");      //переход к созданию структуры
            printf("  [1] создать структуру\n");
            printf(INVITE_STR);
            printf("");
            scanf("%d", &answer);
            repeat = check_answer(answer, 2);
            
            if (answer == 1) goto create_new_struct;
        }
        
        while (repeat == 1) {
            printf("%s\n меню работы с файлом \n%s\n", MENU_LINE, MENU_LINE);
            printf("  [1] добавить запись\n");
            printf("  [2] найти запись\n");
            printf("  [3] сортировать\n");
            printf("  [4] отобразить\n");
            printf("  [5] удаление записи\n");
            printf(INVITE_STR);
            printf("");
            scanf("%d", &answer);
            repeat = check_answer(answer, 5);
        }
        
        switch (answer)
        {
            case 1:
                is_change = add_struct(movies);
                break;
            case 2:
                printf(" введите жанр дл€ поиска: ");
                scanf("%s", genre);
                printf(" введите начало диапазона времени для поиска(hh:mm): ");
                scanf("%d%*c%d", &hour_start, &minute_start);
                printf(" введите окончание диапазона времени для поиска(hh:mm): ");
                scanf("%d%*c%d", &hour_end, &minute_end);
                find_struct(movies, genre, hour_start, hour_end, minute_start, minute_end);
                break;
            case 3:
                is_change = sort_struct(movies);
                break;
            case 4:
                print_all_struct(movies);
                break;
            case 5:
                printf(" введите номер удаляемого элемента: ");
                scanf("%d", &idx);
                is_change = delete_struct(idx, movies);
                break;
        }
        if(is_change == 0){
            do {
                printf("%s\n меню работы с киносеансами\n%s\n", MENU_LINE, MENU_LINE);
                printf("  [1] сохранить в файл\n");
                printf("  [2] отобразить\n");
                printf(INVITE_STR);
                scanf("%d", &answer);
                repeat = check_answer(answer, 2);
            } while (repeat == 1);
            
            switch (answer)
            {
                case 1:
                    save_struct(FILENAME, CNT, movies);
                    break;
                case 2:
                    print_all_struct(movies);
                    break;
            }
        }
    }
    else {
        if (answer == 2) {
        create_new_struct: //функци€ создани€ структуры
            printf("%s\n меню создани€ файла\n%s\n", MENU_LINE, MENU_LINE);
            printf(" введите количество киносеансов: ");
            scanf("%d", &CNT);
            if ((movies = (struct session*)malloc(sizeof(struct session) * CNT)) != NULL) {
                for (int idx = 0; idx < CNT; idx++) {
                    write_stuct(idx, movies);
                }
            }
            else {
                printf(" error: не удалось выделить пам€ть");
                exit(-1);
            }
            
            do {
                printf("%s\n меню работы с киносеансами \n%s\n", MENU_LINE, MENU_LINE);
                printf("  [1] сортировать\n");
                printf("  [2] найти запись\n");
                printf("  [3] сохранить в файл\n");
                printf("  [4] удаление записи\n");
                printf(INVITE_STR);
                scanf("%d", &answer);
                repeat = check_answer(answer, 4);
            } while (repeat == 1);
            
            switch (answer)
            {
                case 1:
                    is_change = sort_struct(movies);
                    break;
                case 2:
                    printf(" введите жанр для поиска: ");
                    scanf("%s", genre);
                    printf(" введите начало диапазона времени дл€ поиска(hh:mm): ");
                    scanf("%d%d", &hour_start, &minute_start);
                    printf(" введите начало диапазона времени дл€ поиска(hh:mm): ");
                    scanf("%d%d", &hour_end, &minute_end);
                    find_struct(movies, genre, hour_start, hour_end, minute_start, minute_end);
                    break;
                case 3:
                    save_struct(FILENAME, CNT, movies);
                    break;
                case 4:
                    printf(" введите номер удаляемого элемента: ");
                    scanf("%d", &idx);
                    is_change = delete_struct(idx, movies);
                    break;
            }
        }
        else printf(" error: некорректный ввод\n");
        if(is_change == 0){
            do {
                printf("%s\n меню работы с киносеансами\n%s\n", MENU_LINE, MENU_LINE);
                printf("  [1] сохранить в файл\n");
                printf("  [2] отобразить\n");
                printf(INVITE_STR);
                scanf("%d", &answer);
                repeat = check_answer(answer, 2);
            } while (repeat == 1);
            
            switch (answer)
            {
                case 1:
                    save_struct(FILENAME, CNT, movies);
                    break;
                case 2:
                    print_all_struct(movies);
                    break;
            }
        }
    }
    free(movies);
    printf("Нажмите любую клавишу...");
    getchar(); getchar();
    
    return 0;
}
