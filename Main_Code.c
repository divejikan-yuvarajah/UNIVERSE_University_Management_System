/*
    University Management System in C

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#ifdef _WIN32
#include <direct.h>
#define MKDIR(a) _mkdir(a)
#else
#include <sys/stat.h>
#define MKDIR(a) mkdir(a, 0777)
#endif

#define MAX_STUDENTS 1000
#define MAX_LECTURERS 200
#define MAX_COURSES 200
#define MAX_FACULTIES 20
#define MAX_DEPARTMENTS 50
#define MAX_NOTICES 100
#define MAX_ATTENDANCE 10000
#define MAX_GRADES 10000
#define MAX_NAME 50
#define MAX_EMAIL 50
#define MAX_PASSWORD 20
#define MAX_ID 20
#define MAX_CODE 10
#define MAX_TITLE 100
#define MAX_LINE 256
#define CAESAR_KEY 3

// Hardcoded credentials (for demo)
#define ADMIN_ID "admin"
#define ADMIN_PASS "admin123"
#define STUDENT1_ID "stu1001"
#define STUDENT1_PASS "stu123"
#define LECTURER1_ID "lec1001"
#define LECTURER1_PASS "lec123"

// File names
// Change these file names from .dat to .txt
#define STUDENT_FILE "students.txt"
#define LECTURER_FILE "lecturers.txt"
#define COURSE_FILE "courses.txt"
#define GRADE_FILE "grades.txt"
#define ATTENDANCE_FILE "attendance.txt"
#define FACULTY_FILE "faculties.txt"
#define NOTICE_FILE "notices.txt"

#define BACKUP_FOLDER "backup/"

// ANSI color codes for dark/light mode
#define ANSI_RESET "\033[0m"
#define ANSI_DARK_BG "\033[40m"
#define ANSI_DARK_FG "\033[97m"
#define ANSI_LIGHT_BG "\033[107m"
#define ANSI_LIGHT_FG "\033[30m"

typedef enum { ROLE_ADMIN, ROLE_STUDENT, ROLE_LECTURER, ROLE_NONE } Role;

// Structures
typedef struct {
    char id[MAX_ID];
    char name[MAX_NAME];
    char email[MAX_EMAIL];
    char faculty[MAX_NAME];
    char department[MAX_NAME];
    float gpa;
} Student;

typedef struct {
    char id[MAX_ID];
    char name[MAX_NAME];
    char email[MAX_EMAIL];
    char faculty[MAX_NAME];
    char department[MAX_NAME];
} Lecturer;

typedef struct {
    char code[MAX_CODE];
    char name[MAX_NAME];
    int credits;
    char lecturer_id[MAX_ID];
    char faculty[MAX_NAME];
    char department[MAX_NAME];
} Course;

typedef struct {
    char faculty[MAX_NAME];
    char department[MAX_NAME];
    char courses[MAX_COURSES][MAX_CODE];
    int course_count;
} Faculty;

typedef struct {
    char student_id[MAX_ID];
    char course_code[MAX_CODE];
    int semester;
    float marks;
    char grade;
} Grade;

typedef struct {
    char student_id[MAX_ID];
    char course_code[MAX_CODE];
    char date[11]; // YYYY-MM-DD
    int present; // 1=present, 0=absent
} Attendance;

typedef struct {
    char title[MAX_TITLE];
    char content[MAX_LINE];
    char date[11];
} Notice;

// Global theme
int dark_mode = 0;

// Utility: Caesar cipher for password encryption/decryption
void caesar_encrypt(char *str, int key) {
    for (int i = 0; str[i]; i++) {
        if (isalpha(str[i]))
            str[i] = ((str[i] - (islower(str[i]) ? 'a' : 'A') + key) % 26) + (islower(str[i]) ? 'a' : 'A');
    }
}
void caesar_decrypt(char *str, int key) {
    caesar_encrypt(str, 26 - key);
}

// Utility: Clear screen
void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Utility: Pause
void pause_screen() {
    printf("Press Enter to continue...");
    getchar();
}

// Utility: Set theme
void set_theme() {
    if (dark_mode)
        printf("%s%s", ANSI_DARK_BG, ANSI_DARK_FG);
    else
        printf("%s%s", ANSI_LIGHT_BG, ANSI_LIGHT_FG);
}

// Utility: Reset theme
void reset_theme() {
    printf("%s", ANSI_RESET);
}

// Utility: Get current date (YYYY-MM-DD)
void get_current_date(char *buf) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(buf, "%04d-%02d-%02d", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday);
}

// Utility: Input string with validation
void input_string(const char *prompt, char *buf, int maxlen) {
    printf("%s", prompt);
    fgets(buf, maxlen, stdin);
    buf[strcspn(buf, "\n")] = 0;
}

// Utility: Input integer with validation
int input_int(const char *prompt, int min, int max) {
    char buf[20];
    int val;
    while (1) {
        printf("%s", prompt);
        fgets(buf, sizeof(buf), stdin);
        if (sscanf(buf, "%d", &val) == 1 && val >= min && val <= max)
            return val;
        printf("Invalid input. Try again.\n");
    }
}

// Utility: Input float with validation
float input_float(const char *prompt, float min, float max) {
    char buf[20];
    float val;
    while (1) {
        printf("%s", prompt);
        fgets(buf, sizeof(buf), stdin);
        if (sscanf(buf, "%f", &val) == 1 && val >= min && val <= max)
            return val;
        printf("Invalid input. Try again.\n");
    }
}

// Utility: Confirm action
int confirm(const char *msg) {
    char c;
    printf("%s (y/n): ", msg);
    c = getchar();
    while (getchar() != '\n');
    return (c == 'y' || c == 'Y');
}
// Auto-save function that creates individual files for each entry
void auto_save_entry(const char* type, const void* data) {
    char filename[100];
    char timestamp[20];
    get_current_date(timestamp);

    if (strcmp(type, "student") == 0) {
        Student* s = (Student*)data;
        sprintf(filename, "student_%s_%s.txt", s->id, timestamp);
        FILE *fp = fopen(filename, "w");
        if (fp) {
            fprintf(fp, "=== STUDENT RECORD ===\n");
            fprintf(fp, "ID: %s\n", s->id);
            fprintf(fp, "Name: %s\n", s->name);
            fprintf(fp, "Email: %s\n", s->email);
            fprintf(fp, "Faculty: %s\n", s->faculty);
            fprintf(fp, "Department: %s\n", s->department);
            fprintf(fp, "GPA: %.2f\n", s->gpa);
            fprintf(fp, "Date Created: %s\n", timestamp);
            fclose(fp);
            printf("Individual student record saved to: %s\n", filename);
        }
    }
    else if (strcmp(type, "lecturer") == 0) {
        Lecturer* l = (Lecturer*)data;
        sprintf(filename, "lecturer_%s_%s.txt", l->id, timestamp);
        FILE *fp = fopen(filename, "w");
        if (fp) {
            fprintf(fp, "=== LECTURER RECORD ===\n");
            fprintf(fp, "ID: %s\n", l->id);
            fprintf(fp, "Name: %s\n", l->name);
            fprintf(fp, "Email: %s\n", l->email);
            fprintf(fp, "Faculty: %s\n", l->faculty);
            fprintf(fp, "Department: %s\n", l->department);
            fprintf(fp, "Date Created: %s\n", timestamp);
            fclose(fp);
            printf("Individual lecturer record saved to: %s\n", filename);
        }
    }
    else if (strcmp(type, "course") == 0) {
        Course* c = (Course*)data;
        sprintf(filename, "course_%s_%s.txt", c->code, timestamp);
        FILE *fp = fopen(filename, "w");
        if (fp) {
            fprintf(fp, "=== COURSE RECORD ===\n");
            fprintf(fp, "Code: %s\n", c->code);
            fprintf(fp, "Name: %s\n", c->name);
            fprintf(fp, "Credits: %d\n", c->credits);
            fprintf(fp, "Lecturer ID: %s\n", c->lecturer_id);
            fprintf(fp, "Faculty: %s\n", c->faculty);
            fprintf(fp, "Department: %s\n", c->department);
            fprintf(fp, "Date Created: %s\n", timestamp);
            fclose(fp);
            printf("Individual course record saved to: %s\n", filename);
        }
    }
}

// =================== FILE HANDLING HELPERS ===================

// Student file helpers
// Student file helpers
int load_students(Student *arr, int max) {
    FILE *fp = fopen(STUDENT_FILE, "r");
    if (!fp) return 0;
    int n = 0;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fp) && n < max) {
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%f",
               arr[n].id, arr[n].name, arr[n].email,
               arr[n].faculty, arr[n].department, &arr[n].gpa);
        n++;
    }
    fclose(fp);
    return n;
}

void save_students(Student *arr, int n) {
    FILE *fp = fopen(STUDENT_FILE, "w");
    if (!fp) return;
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%s,%s,%s,%s,%s,%.2f\n",
                arr[i].id, arr[i].name, arr[i].email,
                arr[i].faculty, arr[i].department, arr[i].gpa);
    }
    fclose(fp);
}


// Lecturer file helpers
// Lecturer file helpers
int load_lecturers(Lecturer *arr, int max) {
    FILE *fp = fopen(LECTURER_FILE, "r");
    if (!fp) return 0;
    int n = 0;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fp) && n < max) {
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^\n]",
               arr[n].id, arr[n].name, arr[n].email,
               arr[n].faculty, arr[n].department);
        n++;
    }
    fclose(fp);
    return n;
}

void save_lecturers(Lecturer *arr, int n) {
    FILE *fp = fopen(LECTURER_FILE, "w");
    if (!fp) return;
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%s,%s,%s,%s,%s\n",
                arr[i].id, arr[i].name, arr[i].email,
                arr[i].faculty, arr[i].department);
    }
    fclose(fp);
}


// Course file helpers
// Course file helpers
int load_courses(Course *arr, int max) {
    FILE *fp = fopen(COURSE_FILE, "r");
    if (!fp) return 0;
    int n = 0;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fp) && n < max) {
        sscanf(line, "%[^,],%[^,],%d,%[^,],%[^,],%[^\n]",
               arr[n].code, arr[n].name, &arr[n].credits,
               arr[n].lecturer_id, arr[n].faculty, arr[n].department);
        n++;
    }
    fclose(fp);
    return n;
}

void save_courses(Course *arr, int n) {
    FILE *fp = fopen(COURSE_FILE, "w");
    if (!fp) return;
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%s,%s,%d,%s,%s,%s\n",
                arr[i].code, arr[i].name, arr[i].credits,
                arr[i].lecturer_id, arr[i].faculty, arr[i].department);
    }
    fclose(fp);
}


// Faculty file helpers (text)
int load_faculties(Faculty *arr, int max) {
    FILE *fp = fopen(FACULTY_FILE, "r");
    if (!fp) return 0;
    int n = 0;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fp) && n < max) {
        sscanf(line, "%[^,],%[^,]", arr[n].faculty, arr[n].department);
        arr[n].course_count = 0;
        n++;
    }
    fclose(fp);
    return n;
}
void save_faculties(Faculty *arr, int n) {
    FILE *fp = fopen(FACULTY_FILE, "w");
    if (!fp) return;
    for (int i = 0; i < n; i++)
        fprintf(fp, "%s,%s\n", arr[i].faculty, arr[i].department);
    fclose(fp);
}

// Grade file helpers
// Grade file helpers
int load_grades(Grade *arr, int max) {
    FILE *fp = fopen(GRADE_FILE, "r");
    if (!fp) return 0;
    int n = 0;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fp) && n < max) {
        sscanf(line, "%[^,],%[^,],%d,%f,%c",
               arr[n].student_id, arr[n].course_code,
               &arr[n].semester, &arr[n].marks, &arr[n].grade);
        n++;
    }
    fclose(fp);
    return n;
}

void save_grades(Grade *arr, int n) {
    FILE *fp = fopen(GRADE_FILE, "w");
    if (!fp) return;
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%s,%s,%d,%.2f,%c\n",
                arr[i].student_id, arr[i].course_code,
                arr[i].semester, arr[i].marks, arr[i].grade);
    }
    fclose(fp);
}


// Attendance file helpers
// Attendance file helpers
int load_attendance(Attendance *arr, int max) {
    FILE *fp = fopen(ATTENDANCE_FILE, "r");
    if (!fp) return 0;
    int n = 0;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fp) && n < max) {
        sscanf(line, "%[^,],%[^,],%[^,],%d",
               arr[n].student_id, arr[n].course_code,
               arr[n].date, &arr[n].present);
        n++;
    }
    fclose(fp);
    return n;
}

void save_attendance(Attendance *arr, int n) {
    FILE *fp = fopen(ATTENDANCE_FILE, "w");
    if (!fp) return;
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%s,%s,%s,%d\n",
                arr[i].student_id, arr[i].course_code,
                arr[i].date, arr[i].present);
    }
    fclose(fp);
}


// Notice file helpers (text)
int load_notices(Notice *arr, int max) {
    FILE *fp = fopen(NOTICE_FILE, "r");
    if (!fp) return 0;
    int n = 0;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fp) && n < max) {
        sscanf(line, "%[^|]|%[^|]|%[^\n]", arr[n].title, arr[n].content, arr[n].date);
        n++;
    }
    fclose(fp);
    return n;
}
void save_notices(Notice *arr, int n) {
    FILE *fp = fopen(NOTICE_FILE, "w");
    if (!fp) return;
    for (int i = 0; i < n; i++)
        fprintf(fp, "%s|%s|%s\n", arr[i].title, arr[i].content, arr[i].date);
    fclose(fp);
}

// =================== LOGIN SYSTEM ===================

int check_credentials(const char *id, const char *pass, Role *role) {
    char enc_admin_pass[MAX_PASSWORD], enc_student1_pass[MAX_PASSWORD], enc_lecturer1_pass[MAX_PASSWORD];
    strcpy(enc_admin_pass, ADMIN_PASS);
    strcpy(enc_student1_pass, STUDENT1_PASS);
    strcpy(enc_lecturer1_pass, LECTURER1_PASS);
    caesar_encrypt(enc_admin_pass, CAESAR_KEY);
    caesar_encrypt(enc_student1_pass, CAESAR_KEY);
    caesar_encrypt(enc_lecturer1_pass, CAESAR_KEY);

    char enc_input[MAX_PASSWORD];
    strcpy(enc_input, pass);
    caesar_encrypt(enc_input, CAESAR_KEY);

    if (strcmp(id, ADMIN_ID) == 0 && strcmp(enc_input, enc_admin_pass) == 0) {
        *role = ROLE_ADMIN;
        return 1;
    }
    if (strcmp(id, STUDENT1_ID) == 0 && strcmp(enc_input, enc_student1_pass) == 0) {
        *role = ROLE_STUDENT;
        return 1;
    }
    if (strcmp(id, LECTURER1_ID) == 0 && strcmp(enc_input, enc_lecturer1_pass) == 0) {
        *role = ROLE_LECTURER;
        return 1;
    }
    *role = ROLE_NONE;
    return 0;
}

void login_panel(Role *role, char *user_id) {
    int panel;
    char id[MAX_ID], pass[MAX_PASSWORD];
    while (1) {
        clear_screen();
        set_theme();
        printf("\n");
        printf(" \033[1;31m - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - +\033[0m");
        printf("\n\n");
        // Main heading in green (ASCII art)
        printf("\033[1;32m ##     ##    ##        ##    ########      ##     ##   ########   ########   ########   ######## \n");
        printf("\033[1;32m ##     ##    ## #      ##       ##         ##     ##   ##         ##    ##   ##         ##        \n");
        printf("\033[1;32m ##     ##    ##   #    ##       ##    ###  ##    ##    ########   ########   ########   ########\n");
        printf("\033[1;32m ##     ##    ##     #  ##       ##           ## ##     ##         ##   ##          ##   ##     \n");
        printf("\033[1;32m #########    ##       ###    ########          #       ########   ##    ##   ########   ######## \n");
        // Bottom border line in red
        printf(" \033[1;31m - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + \033[0m");
        printf("\n\n");
        printf("\t\t\t\t|||==== University Management System ====|||\n");
        printf("\n");

        printf("\t\t\t==== SOUTH EASTERN UNIVERSITY OF SRI LANKA (SEUSL) ====\n");
        printf("\n");
        printf("Select Panel:\n");
        printf("1. Admin\n2. Student\n3. Lecturer\n4. Exit\n");
        reset_theme();
        panel = input_int("Enter choice: ", 1, 4);
        if (panel == 4) exit(0);

        input_string("Enter ID: ", id, MAX_ID);
        input_string("Enter Password: ", pass, MAX_PASSWORD);

        Role r;
        if (panel == 1 && check_credentials(id, pass, &r) && r == ROLE_ADMIN) {
            *role = ROLE_ADMIN;
            strcpy(user_id, id);
            break;
        } else if (panel == 2 && check_credentials(id, pass, &r) && r == ROLE_STUDENT) {
            *role = ROLE_STUDENT;
            strcpy(user_id, id);
            break;
        } else if (panel == 3 && check_credentials(id, pass, &r) && r == ROLE_LECTURER) {
            *role = ROLE_LECTURER;
            strcpy(user_id, id);
            break;
        } else {
            printf("Invalid credentials or role. Try again.\n");
            pause_screen();
        }
    }
}

// =================== NOTIFICATIONS PANEL ===================

void view_notices() {
    Notice notices[MAX_NOTICES];
    int n = load_notices(notices, MAX_NOTICES);
    if (n == 0) {
        printf("No notices available.\n");
        return;
    }
    printf("---- Announcements ----\n");
    for (int i = 0; i < n; i++) {
        printf("[%s] %s\n%s\n\n", notices[i].date, notices[i].title, notices[i].content);
    }
}

void post_notice() {
    Notice notices[MAX_NOTICES];
    int n = load_notices(notices, MAX_NOTICES);
    if (n >= MAX_NOTICES) {
        printf("Notice board full.\n");
        return;
    }
    input_string("Enter notice title: ", notices[n].title, MAX_TITLE);
    input_string("Enter notice content: ", notices[n].content, MAX_LINE);
    get_current_date(notices[n].date);
    n++;
    save_notices(notices, n);
    printf("Notice posted.\n");
}

// =================== STUDENT MANAGEMENT ===================

void add_student() {
    Student students[MAX_STUDENTS];
    int n = load_students(students, MAX_STUDENTS);
    if (n >= MAX_STUDENTS) {
        printf("Student database full.\n");
        return;
    }
    Student s;
    input_string("Enter Student ID: ", s.id, MAX_ID);
    for (int i = 0; i < n; i++) {
        if (strcmp(students[i].id, s.id) == 0) {
            printf("Student ID already exists.\n");
            return;
        }
    }
    input_string("Enter Name: ", s.name, MAX_NAME);
    input_string("Enter Email: ", s.email, MAX_EMAIL);
    input_string("Enter Faculty: ", s.faculty, MAX_NAME);
    input_string("Enter Department: ", s.department, MAX_NAME);
    s.gpa = 0.0;
    students[n++] = s;
    save_students(students, n);
    auto_save_entry("student", &s);  // NEW LINE ADDED
    printf("Student added.\n");
}


void edit_student() {
    Student students[MAX_STUDENTS];
    int n = load_students(students, MAX_STUDENTS);
    char id[MAX_ID];
    input_string("Enter Student ID to edit: ", id, MAX_ID);
    for (int i = 0; i < n; i++) {
        if (strcmp(students[i].id, id) == 0) {
            printf("Editing %s (%s)\n", students[i].name, students[i].id);
            input_string("Enter new Name: ", students[i].name, MAX_NAME);
            input_string("Enter new Email: ", students[i].email, MAX_EMAIL);
            input_string("Enter new Faculty: ", students[i].faculty, MAX_NAME);
            input_string("Enter new Department: ", students[i].department, MAX_NAME);
            save_students(students, n);
            printf("Student updated.\n");
            return;
        }
    }
    printf("Student not found.\n");
}

void delete_student() {
    Student students[MAX_STUDENTS];
    int n = load_students(students, MAX_STUDENTS);
    char id[MAX_ID];
    input_string("Enter Student ID to delete: ", id, MAX_ID);
    for (int i = 0; i < n; i++) {
        if (strcmp(students[i].id, id) == 0) {
            if (!confirm("Are you sure you want to delete this student?")) return;
            for (int j = i; j < n-1; j++)
                students[j] = students[j+1];
            n--;
            save_students(students, n);
            printf("Student deleted.\n");
            return;
        }
    }
    printf("Student not found.\n");
}

void view_students() {
    Student students[MAX_STUDENTS];
    int n = load_students(students, MAX_STUDENTS);

    if (n == 0) {
        printf("\n+--------------------------------------+\n");
        printf("|          No students found!          |\n");
        printf("+--------------------------------------+\n");
        return;
    }

    printf("\n+============+=======================+=========================+======================+======================+======================+\n");
    printf("|                                                    STUDENT RECORDS                                                                       |\n");
    printf("+============+=======================+=========================+======================+======================+======================+\n");
    printf("| Student ID |         Name          |           Email         |        Faculty       |      Department      |          GPA         |\n");
    printf("+============+=======================+=========================+======================+======================+======================+\n");

    for (int i = 0; i < n; i++) {
        printf("| %-10s | %-21s | %-23s | %-20s | %-20s |        %.2f        |\n",
               students[i].id,
               students[i].name,
               students[i].email,
               students[i].faculty,
               students[i].department,
               students[i].gpa);
    }

    printf("+============+=======================+=========================+======================+======================+======================+\n");
    printf("Total Students: %d\n\n", n);
}

void search_student() {
    Student students[MAX_STUDENTS];
    int n = load_students(students, MAX_STUDENTS);
    char key[MAX_NAME];
    int found = 0;

    input_string("Enter name or ID to search: ", key, MAX_NAME);

    printf("\n+============+=======================+=========================+======================+======================+======================+\n");
    printf("|                                                   SEARCH RESULTS                                                                         |\n");
    printf("+============+=======================+=========================+======================+======================+======================+\n");
    printf("| Student ID |         Name          |           Email         |        Faculty       |      Department      |          GPA         |\n");
    printf("+============+=======================+=========================+======================+======================+======================+\n");

    for (int i = 0; i < n; i++) {
        if (strstr(students[i].name, key) || strstr(students[i].id, key)) {
            printf("| %-10s | %-21s | %-23s | %-20s | %-20s |        %.2f        |\n",
                   students[i].id,
                   students[i].name,
                   students[i].email,
                   students[i].faculty,
                   students[i].department,
                   students[i].gpa);
            found++;
        }
    }

    if (found == 0) {
        printf("|                                                 No matching students found!                                                         |\n");
    }

    printf("+============+=======================+=========================+======================+======================+======================+\n");
    printf("Search Results: %d student(s) found\n\n", found);
}



// =================== LECTURER MANAGEMENT ===================

void add_lecturer() {
    Lecturer lecturers[MAX_LECTURERS];
    int n = load_lecturers(lecturers, MAX_LECTURERS);
    if (n >= MAX_LECTURERS) {
        printf("Lecturer database full.\n");
        return;
    }
    Lecturer l;
    input_string("Enter Lecturer ID: ", l.id, MAX_ID);
    for (int i = 0; i < n; i++) {
        if (strcmp(lecturers[i].id, l.id) == 0) {
            printf("Lecturer ID already exists.\n");
            return;
        }
    }
    input_string("Enter Name: ", l.name, MAX_NAME);
    input_string("Enter Email: ", l.email, MAX_EMAIL);
    input_string("Enter Faculty: ", l.faculty, MAX_NAME);
    input_string("Enter Department: ", l.department, MAX_NAME);
    lecturers[n++] = l;
    save_lecturers(lecturers, n);
    auto_save_entry("lecturer", &l);  // NEW LINE ADDED
    printf("Lecturer added.\n");
}


void edit_lecturer() {
    Lecturer lecturers[MAX_LECTURERS];
    int n = load_lecturers(lecturers, MAX_LECTURERS);
    char id[MAX_ID];
    input_string("Enter Lecturer ID to edit: ", id, MAX_ID);
    for (int i = 0; i < n; i++) {
        if (strcmp(lecturers[i].id, id) == 0) {
            printf("Editing %s (%s)\n", lecturers[i].name, lecturers[i].id);
            input_string("Enter new Name: ", lecturers[i].name, MAX_NAME);
            input_string("Enter new Email: ", lecturers[i].email, MAX_EMAIL);
            input_string("Enter new Faculty: ", lecturers[i].faculty, MAX_NAME);
            input_string("Enter new Department: ", lecturers[i].department, MAX_NAME);
            save_lecturers(lecturers, n);
            printf("Lecturer updated.\n");
            return;
        }
    }
    printf("Lecturer not found.\n");
}

void delete_lecturer() {
    Lecturer lecturers[MAX_LECTURERS];
    int n = load_lecturers(lecturers, MAX_LECTURERS);
    char id[MAX_ID];
    input_string("Enter Lecturer ID to delete: ", id, MAX_ID);
    for (int i = 0; i < n; i++) {
        if (strcmp(lecturers[i].id, id) == 0) {
            if (!confirm("Are you sure you want to delete this lecturer?")) return;
            for (int j = i; j < n-1; j++)
                lecturers[j] = lecturers[j+1];
            n--;
            save_lecturers(lecturers, n);
            printf("Lecturer deleted.\n");
            return;
        }
    }
    printf("Lecturer not found.\n");
}

void view_lecturers() {
    Lecturer lecturers[MAX_LECTURERS];
    int n = load_lecturers(lecturers, MAX_LECTURERS);

    if (n == 0) {
        printf("\n+--------------------------------------+\n");
        printf("|          No lecturers found!         |\n");
        printf("+--------------------------------------+\n");
        return;
    }

    printf("\n+============+=======================+=========================+======================+======================+\n");
    printf("|                                            LECTURER RECORDS                                                |\n");
    printf("+============+=======================+=========================+======================+======================+\n");
    printf("| Lecturer ID|         Name          |           Email         |        Faculty       |      Department      |\n");
    printf("+============+=======================+=========================+======================+======================+\n");

    for (int i = 0; i < n; i++) {
        printf("| %-10s | %-21s | %-23s | %-20s | %-20s |\n",
               lecturers[i].id,
               lecturers[i].name,
               lecturers[i].email,
               lecturers[i].faculty,
               lecturers[i].department);
    }

    printf("+============+=======================+=========================+======================+======================+\n");
    printf("Total Lecturers: %d\n\n", n);
}


void search_lecturer() {
    Lecturer lecturers[MAX_LECTURERS];
    int n = load_lecturers(lecturers, MAX_LECTURERS);
    char key[MAX_NAME];
    int found = 0;

    input_string("Enter name or ID to search: ", key, MAX_NAME);

    printf("\n+============+=======================+=========================+======================+======================+\n");
    printf("|                                              SEARCH RESULTS                                                 |\n");
    printf("+============+=======================+=========================+======================+======================+\n");
    printf("| Lecturer ID|         Name          |           Email         |        Faculty       |      Department      |\n");
    printf("+============+=======================+=========================+======================+======================+\n");

    for (int i = 0; i < n; i++) {
        if (strstr(lecturers[i].name, key) || strstr(lecturers[i].id, key)) {
            printf("| %-10s | %-21s | %-23s | %-20s | %-20s |\n",
                   lecturers[i].id,
                   lecturers[i].name,
                   lecturers[i].email,
                   lecturers[i].faculty,
                   lecturers[i].department);
            found++;
        }
    }

    if (found == 0) {
        printf("|                                      No matching lecturers found!                                         |\n");
    }

    printf("+============+=======================+=========================+======================+======================+\n");
    printf("Search Results: %d lecturer(s) found\n\n", found);
}


// =================== FACULTY MANAGEMENT ===================

void list_faculties() {
    Faculty faculties[MAX_FACULTIES];
    int n = load_faculties(faculties, MAX_FACULTIES);
    printf("Faculty\tDepartment\n");
    for (int i = 0; i < n; i++) {
        printf("%s\t%s\n", faculties[i].faculty, faculties[i].department);
    }
}

void add_faculty() {
    Faculty faculties[MAX_FACULTIES];
    int n = load_faculties(faculties, MAX_FACULTIES);
    if (n >= MAX_FACULTIES) {
        printf("Faculty database full.\n");
        return;
    }
    input_string("Enter Faculty Name: ", faculties[n].faculty, MAX_NAME);
    input_string("Enter Department Name: ", faculties[n].department, MAX_NAME);
    faculties[n].course_count = 0;
    n++;
    save_faculties(faculties, n);
    printf("Faculty added.\n");
}



// =================== COURSE MANAGEMENT ===================

void add_course() {
    Course courses[MAX_COURSES];
    int n = load_courses(courses, MAX_COURSES);
    if (n >= MAX_COURSES) {
        printf("Course database full.\n");
        return;
    }
    Course c;
    input_string("Enter Course Code: ", c.code, MAX_CODE);
    for (int i = 0; i < n; i++) {
        if (strcmp(courses[i].code, c.code) == 0) {
            printf("Course code already exists.\n");
            return;
        }
    }
    input_string("Enter Course Name: ", c.name, MAX_NAME);
    c.credits = input_int("Enter Credits: ", 1, 10);
    input_string("Enter Lecturer ID: ", c.lecturer_id, MAX_ID);
    input_string("Enter Faculty: ", c.faculty, MAX_NAME);
    input_string("Enter Department: ", c.department, MAX_NAME);
    courses[n++] = c;
    save_courses(courses, n);
    auto_save_entry("course", &c);  // NEW LINE ADDED
    printf("Course added.\n");
}


void edit_course() {
    Course courses[MAX_COURSES];
    int n = load_courses(courses, MAX_COURSES);
    char code[MAX_CODE];
    input_string("Enter Course Code to edit: ", code, MAX_CODE);
    for (int i = 0; i < n; i++) {
        if (strcmp(courses[i].code, code) == 0) {
            printf("Editing %s (%s)\n", courses[i].name, courses[i].code);
            input_string("Enter new Name: ", courses[i].name, MAX_NAME);
            courses[i].credits = input_int("Enter new Credits: ", 1, 10);
            input_string("Enter new Lecturer ID: ", courses[i].lecturer_id, MAX_ID);
            input_string("Enter new Faculty: ", courses[i].faculty, MAX_NAME);
            input_string("Enter new Department: ", courses[i].department, MAX_NAME);
            save_courses(courses, n);
            printf("Course updated.\n");
            return;
        }
    }
    printf("Course not found.\n");
}

void delete_course() {
    Course courses[MAX_COURSES];
    int n = load_courses(courses, MAX_COURSES);
    char code[MAX_CODE];
    input_string("Enter Course Code to delete: ", code, MAX_CODE);
    for (int i = 0; i < n; i++) {
        if (strcmp(courses[i].code, code) == 0) {
            if (!confirm("Are you sure you want to delete this course?")) return;
            for (int j = i; j < n-1; j++)
                courses[j] = courses[j+1];
            n--;
            save_courses(courses, n);
            printf("Course deleted.\n");
            return;
        }
    }
    printf("Course not found.\n");
}

void view_courses() {
    Course courses[MAX_COURSES];
    int n = load_courses(courses, MAX_COURSES);

    if (n == 0) {
        printf("\n+--------------------------------------+\n");
        printf("|           No courses found!          |\n");
        printf("+--------------------------------------+\n");
        return;
    }

    printf("\n+============+=======================+==========+=============+======================+======================+\n");
    printf("|                                                 COURSE RECORDS                                                |\n");
    printf("+============+=======================+==========+=============+======================+======================+\n");
    printf("| Course Code|      Course Name      | Credits  | Lecturer ID |        Faculty       |      Department      |\n");
    printf("+============+=======================+==========+=============+======================+======================+\n");

    for (int i = 0; i < n; i++) {
        printf("| %-10s | %-21s |    %2d    | %-11s | %-20s | %-20s |\n",
               courses[i].code,
               courses[i].name,
               courses[i].credits,
               courses[i].lecturer_id,
               courses[i].faculty,
               courses[i].department);
    }

    printf("+============+=======================+==========+=============+======================+======================+\n");
    printf("Total Courses: %d\n\n", n);
}


void search_course() {
    Course courses[MAX_COURSES];
    int n = load_courses(courses, MAX_COURSES);
    char key[MAX_NAME];
    int found = 0;

    input_string("Enter course name or code to search: ", key, MAX_NAME);

    printf("\n+============+=======================+==========+=============+======================+======================+\n");
    printf("|                                                SEARCH RESULTS                                                 |\n");
    printf("+============+=======================+==========+=============+======================+======================+\n");
    printf("| Course Code|      Course Name      | Credits  | Lecturer ID |        Faculty       |      Department      |\n");
    printf("+============+=======================+==========+=============+======================+======================+\n");

    for (int i = 0; i < n; i++) {
        if (strstr(courses[i].name, key) || strstr(courses[i].code, key)) {
            printf("| %-10s | %-21s |    %2d    | %-11s | %-20s | %-20s |\n",
                   courses[i].code,
                   courses[i].name,
                   courses[i].credits,
                   courses[i].lecturer_id,
                   courses[i].faculty,
                   courses[i].department);
            found++;
        }
    }

    if (found == 0) {
        printf("|                                        No matching courses found!                                          |\n");
    }

    printf("+============+=======================+==========+=============+======================+======================+\n");
    printf("Search Results: %d course(s) found\n\n", found);
}


// =================== MARKS & GRADE CALCULATION ===================

char calc_grade(float marks) {
    if (marks >= 90) return 'A';
    if (marks >= 80) return 'B';
    if (marks >= 70) return 'C';
    if (marks >= 60) return 'D';
    if (marks >= 50) return 'E';
    return 'F';
}

void enter_marks() {
    Grade grades[MAX_GRADES];
    int n = load_grades(grades, MAX_GRADES);
    if (n >= MAX_GRADES) {
        printf("Grade database full.\n");
        return;
    }
    Grade g;
    input_string("Enter Student ID: ", g.student_id, MAX_ID);
    input_string("Enter Course Code: ", g.course_code, MAX_CODE);
    g.semester = input_int("Enter Semester: ", 1, 12);
    g.marks = input_float("Enter Marks (0-100): ", 0, 100);
    g.grade = calc_grade(g.marks);
    grades[n++] = g;
    save_grades(grades, n);
    printf("Marks and grade entered. Grade: %c\n", g.grade);
}

// =================== GPA CALCULATOR ===================

float compute_gpa(const char *student_id) {
    Grade grades[MAX_GRADES];
    int n = load_grades(grades, MAX_GRADES);
    Course courses[MAX_COURSES];
    int m = load_courses(courses, MAX_COURSES);
    float total_points = 0, total_credits = 0;
    for (int i = 0; i < n; i++) {
        if (strcmp(grades[i].student_id, student_id) == 0) {
            int credits = 0;
            for (int j = 0; j < m; j++) {
                if (strcmp(courses[j].code, grades[i].course_code) == 0) {
                    credits = courses[j].credits;
                    break;
                }
            }
            float point = 0;
            switch (grades[i].grade) {
                case 'A': point = 4.0; break;
                case 'B': point = 3.0; break;
                case 'C': point = 2.0; break;
                case 'D': point = 1.0; break;
                case 'E': point = 0.5; break;
                default: point = 0.0;
            }
            total_points += point * credits;
            total_credits += credits;
        }
    }
    if (total_credits == 0) return 0.0;
    return total_points / total_credits;
}

void update_student_gpa(const char *student_id) {
    Student students[MAX_STUDENTS];
    int n = load_students(students, MAX_STUDENTS);
    float gpa = compute_gpa(student_id);
    for (int i = 0; i < n; i++) {
        if (strcmp(students[i].id, student_id) == 0) {
            students[i].gpa = gpa;
            save_students(students, n);
            return;
        }
    }
}

// =================== TRANSCRIPT GENERATOR ===================

void generate_transcript(const char *student_id) {
    Student students[MAX_STUDENTS];
    int n = load_students(students, MAX_STUDENTS);
    int found = 0;
    Student s;
    for (int i = 0; i < n; i++) {
        if (strcmp(students[i].id, student_id) == 0) {
            s = students[i];
            found = 1;
            break;
        }
    }
    if (!found) {
        printf("Student not found.\n");
        return;
    }
    Grade grades[MAX_GRADES];
    int m = load_grades(grades, MAX_GRADES);
    Course courses[MAX_COURSES];
    int k = load_courses(courses, MAX_COURSES);

    char filename[64];
    sprintf(filename, "transcript_%s.txt", student_id);
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Failed to create transcript file.\n");
        return;
    }
    printf("==== Transcript for %s (%s) ====\n", s.name, s.id);
    fprintf(fp, "==== Transcript for %s (%s) ====\n", s.name, s.id);
    printf("Faculty: %s\tDepartment: %s\n", s.faculty, s.department);
    fprintf(fp, "Faculty: %s\tDepartment: %s\n", s.faculty, s.department);
    printf("Course\tMarks\tGrade\n");
    fprintf(fp, "Course\tMarks\tGrade\n");
    for (int i = 0; i < m; i++) {
        if (strcmp(grades[i].student_id, student_id) == 0) {
            char cname[MAX_NAME] = "";
            for (int j = 0; j < k; j++) {
                if (strcmp(courses[j].code, grades[i].course_code) == 0) {
                    strcpy(cname, courses[j].name);
                    break;
                }
            }
            printf("%s (%s)\t%.2f\t%c\n", cname, grades[i].course_code, grades[i].marks, grades[i].grade);
            fprintf(fp, "%s (%s)\t%.2f\t%c\n", cname, grades[i].course_code, grades[i].marks, grades[i].grade);
        }
    }
    float gpa = compute_gpa(student_id);
    printf("GPA: %.2f\n", gpa);
    fprintf(fp, "GPA: %.2f\n", gpa);
    fclose(fp);
    printf("Transcript saved to %s\n", filename);
}

// =================== PROGRESS CHART ===================

void show_progress_chart(const char *student_id) {
    Grade grades[MAX_GRADES];
    int n = load_grades(grades, MAX_GRADES);
    float gpa_per_sem[12] = {0};
    int sem_count[12] = {0};
    Course courses[MAX_COURSES];
    int m = load_courses(courses, MAX_COURSES);

    for (int i = 0; i < n; i++) {
        if (strcmp(grades[i].student_id, student_id) == 0) {
            int sem = grades[i].semester;
            int credits = 0;
            for (int j = 0; j < m; j++) {
                if (strcmp(courses[j].code, grades[i].course_code) == 0) {
                    credits = courses[j].credits;
                    break;
                }
            }
            float point = 0;
            switch (grades[i].grade) {
                case 'A': point = 4.0; break;
                case 'B': point = 3.0; break;
                case 'C': point = 2.0; break;
                case 'D': point = 1.0; break;
                case 'E': point = 0.5; break;
                default: point = 0.0;
            }
            gpa_per_sem[sem-1] += point * credits;
            sem_count[sem-1] += credits;
        }
    }
    printf("Semester GPA Progression:\n");
    for (int i = 0; i < 12; i++) {
        if (sem_count[i] > 0) {
            float gpa = gpa_per_sem[i] / sem_count[i];
            printf("Sem %2d: [", i+1);
            int bars = (int)(gpa * 10 / 4.0);
            for (int j = 0; j < bars; j++) printf("#");
            for (int j = bars; j < 10; j++) printf(" ");
            printf("] %.2f\n", gpa);
        }
    }
    if (confirm("Export chart to .txt?")) {
        char filename[64];
        sprintf(filename, "progress_%s.txt", student_id);
        FILE *fp = fopen(filename, "w");
        if (!fp) {
            printf("Failed to create chart file.\n");
            return;
        }
        for (int i = 0; i < 12; i++) {
            if (sem_count[i] > 0) {
                float gpa = gpa_per_sem[i] / sem_count[i];
                fprintf(fp, "Sem %2d: [", i+1);
                int bars = (int)(gpa * 10 / 4.0);
                for (int j = 0; j < bars; j++) fprintf(fp, "#");
                for (int j = bars; j < 10; j++) fprintf(fp, " ");
                fprintf(fp, "] %.2f\n", gpa);
            }
        }
        fclose(fp);
        printf("Chart exported to %s\n", filename);
    }
}

// =================== ATTENDANCE SYSTEM ===================

void mark_attendance() {
    Attendance att[MAX_ATTENDANCE];
    int n = load_attendance(att, MAX_ATTENDANCE);
    if (n >= MAX_ATTENDANCE) {
        printf("Attendance database full.\n");
        return;
    }
    Attendance a;
    input_string("Enter Student ID: ", a.student_id, MAX_ID);
    input_string("Enter Course Code: ", a.course_code, MAX_CODE);
    input_string("Enter Date (YYYY-MM-DD): ", a.date, 11);
    a.present = input_int("Present? (1=Yes, 0=No): ", 0, 1);
    att[n++] = a;
    save_attendance(att, n);
    printf("Attendance marked.\n");
}

void view_attendance() {
    Attendance attendance[MAX_ATTENDANCE];
    int n = load_attendance(attendance, MAX_ATTENDANCE);

    if (n == 0) {
        printf("\n+--------------------------------------+\n");
        printf("|         No attendance found!         |\n");
        printf("+--------------------------------------+\n");
        return;
    }

    printf("\n+============+=============+=============+==========+\n");
    printf("|                  ATTENDANCE RECORDS                |\n");
    printf("+============+=============+=============+==========+\n");
    printf("| Student ID | Course Code |    Date     | Present  |\n");
    printf("+============+=============+=============+==========+\n");

    for (int i = 0; i < n; i++) {
        printf("| %-10s | %-11s | %-11s |    %s    |\n",
               attendance[i].student_id,
               attendance[i].course_code,
               attendance[i].date,
               attendance[i].present ? "Yes" : "No");
    }

    printf("+============+=============+=============+==========+\n");
    printf("Total Attendance Records: %d\n\n", n);
}


// =================== SEARCH & FILTER ENGINE ===================

void search_filter_menu() {
    printf("1. Search Student\n2. Search Lecturer\n3. Search Course\n");
    int ch = input_int("Enter choice: ", 1, 3);
    if (ch == 1) search_student();
    else if (ch == 2) search_lecturer();
    else if (ch == 3) search_course();
}

// =================== ADMIN ANALYTICS ===================

void admin_analytics() {
    Student students[MAX_STUDENTS];
    int n = load_students(students, MAX_STUDENTS);
    Lecturer lecturers[MAX_LECTURERS];
    int m = load_lecturers(lecturers, MAX_LECTURERS);
    float total_gpa = 0, max_gpa = 0;
    char top_id[MAX_ID] = "";
    for (int i = 0; i < n; i++) {
        total_gpa += students[i].gpa;
        if (students[i].gpa > max_gpa) {
            max_gpa = students[i].gpa;
            strcpy(top_id, students[i].id);
        }
    }
    printf("Total Students: %d\n", n);
    printf("Total Lecturers: %d\n", m);
    printf("Average GPA: %.2f\n", n ? total_gpa/n : 0.0);
    if (n)
        printf("Top Performer: %s (GPA: %.2f)\n", top_id, max_gpa);
}

// =================== BACKUP & RESTORE ===================

void backup_files() {
    MKDIR(BACKUP_FOLDER);
    char cmd[256];
    sprintf(cmd, "cp %s %s", STUDENT_FILE, BACKUP_FOLDER);
    system(cmd);
    sprintf(cmd, "cp %s %s", LECTURER_FILE, BACKUP_FOLDER);
    system(cmd);
    sprintf(cmd, "cp %s %s", FACULTY_FILE, BACKUP_FOLDER);
    system(cmd);
    sprintf(cmd, "cp %s %s", COURSE_FILE, BACKUP_FOLDER);
    system(cmd);
    sprintf(cmd, "cp %s %s", GRADE_FILE, BACKUP_FOLDER);
    system(cmd);
    sprintf(cmd, "cp %s %s", ATTENDANCE_FILE, BACKUP_FOLDER);
    system(cmd);
    sprintf(cmd, "cp %s %s", NOTICE_FILE, BACKUP_FOLDER);
    system(cmd);
    printf("Backup completed to %s\n", BACKUP_FOLDER);
}

void restore_files() {
    char cmd[256];
    sprintf(cmd, "cp %s%s %s", BACKUP_FOLDER, STUDENT_FILE, STUDENT_FILE);
    system(cmd);
    sprintf(cmd, "cp %s%s %s", BACKUP_FOLDER, LECTURER_FILE, LECTURER_FILE);
    system(cmd);
    sprintf(cmd, "cp %s%s %s", BACKUP_FOLDER, FACULTY_FILE, FACULTY_FILE);
    system(cmd);
    sprintf(cmd, "cp %s%s %s", BACKUP_FOLDER, COURSE_FILE, COURSE_FILE);
    system(cmd);
    sprintf(cmd, "cp %s%s %s", BACKUP_FOLDER, GRADE_FILE, GRADE_FILE);
    system(cmd);
    sprintf(cmd, "cp %s%s %s", BACKUP_FOLDER, ATTENDANCE_FILE, ATTENDANCE_FILE);
    system(cmd);
    sprintf(cmd, "cp %s%s %s", BACKUP_FOLDER, NOTICE_FILE, NOTICE_FILE);
    system(cmd);
    printf("Restore completed from %s\n", BACKUP_FOLDER);
}

// =================== DARK MODE TOGGLE ===================

void toggle_dark_mode() {
    dark_mode = !dark_mode;
    set_theme();
    printf("Theme changed to %s mode.\n", dark_mode ? "Dark" : "Light");
    reset_theme();
}

// =================== MENUS ===================

void admin_menu() {
    int ch;
    do {
        clear_screen();
        set_theme();
        printf("==== Admin Panel ====\n");
        printf("1. Student Management\n2. Lecturer Management\n3. Faculty Management\n4. Course Management\n5. Marks & Grades\n6. Analytics\n7. Notices\n8. Backup/Restore\n9. Search/Filter\n10. Theme Toggle\n0. Logout\n");
        reset_theme();
        ch = input_int("Enter choice: ", 0, 10);
        switch (ch) {
            case 1:
                printf("1. Add Student\n2. Edit Student\n3. Delete Student\n4. View Students\n5. Search Student\n0. Back\n");
                switch (input_int("Enter choice: ", 0, 5)) {
                    case 1: add_student(); break;
                    case 2: edit_student(); break;
                    case 3: delete_student(); break;
                    case 4: view_students(); break;
                    case 5: search_student(); break;
                }
                break;
            case 2:
                printf("1. Add Lecturer\n2. Edit Lecturer\n3. Delete Lecturer\n4. View Lecturers\n5. Search Lecturer\n0. Back\n");
                switch (input_int("Enter choice: ", 0, 5)) {
                    case 1: add_lecturer(); break;
                    case 2: edit_lecturer(); break;
                    case 3: delete_lecturer(); break;
                    case 4: view_lecturers(); break;
                    case 5: search_lecturer(); break;
                }
                break;
            case 3:
                printf("1. List Faculties\n2. Add Faculty\n0. Back\n");
                switch (input_int("Enter choice: ", 0, 2)) {
                    case 1: list_faculties(); break;
                    case 2: add_faculty(); break;
                }
                break;
            case 4:
                printf("1. Add Course\n2. Edit Course\n3. Delete Course\n4. View Courses\n5. Search Course\n0. Back\n");
                switch (input_int("Enter choice: ", 0, 5)) {
                    case 1: add_course(); break;
                    case 2: edit_course(); break;
                    case 3: delete_course(); break;
                    case 4: view_courses(); break;
                    case 5: search_course(); break;
                }
                break;
            case 5:
                printf("1. Enter Marks\n0. Back\n");
                switch (input_int("Enter choice: ", 0, 1)) {
                    case 1: enter_marks(); break;
                }
                break;
            case 6:
                admin_analytics();
                break;
            case 7:
                printf("1. Post Notice\n2. View Notices\n0. Back\n");
                switch (input_int("Enter choice: ", 0, 2)) {
                    case 1: post_notice(); break;
                    case 2: view_notices(); break;
                }
                break;
            case 8:
                printf("1. Backup\n2. Restore\n0. Back\n");
                switch (input_int("Enter choice: ", 0, 2)) {
                    case 1: backup_files(); break;
                    case 2: restore_files(); break;
                }
                break;
            case 9:
                search_filter_menu();
                break;
            case 10:
                toggle_dark_mode();
                break;
        }
        if (ch != 0) pause_screen();
    } while (ch != 0);
}

void student_menu(const char *student_id) {
    int ch;
    do {
        clear_screen();
        set_theme();
        printf("==== Student Panel ====\n");
        printf("1. View Profile\n2. View Courses\n3. View Grades\n4. GPA Calculator\n5. Transcript\n6. Progress Chart\n7. Attendance\n8. View Notices\n9. Theme Toggle\n0. Logout\n");
        reset_theme();
        ch = input_int("Enter choice: ", 0, 9);
        switch (ch) {
            case 1:
                search_student();
                break;
            case 2:
                view_courses();
                break;
            case 3:
                printf("Course\tMarks\tGrade\n");
                {
                    Grade grades[MAX_GRADES];
                    int n = load_grades(grades, MAX_GRADES);
                    for (int i = 0; i < n; i++) {
                        if (strcmp(grades[i].student_id, student_id) == 0)
                            printf("%s\t%.2f\t%c\n", grades[i].course_code, grades[i].marks, grades[i].grade);
                    }
                }
                break;
            case 4:
                update_student_gpa(student_id);
                printf("GPA: %.2f\n", compute_gpa(student_id));
                break;
            case 5:
                generate_transcript(student_id);
                break;
            case 6:
                show_progress_chart(student_id);
                break;
            case 7:
                view_attendance();
                break;
            case 8:
                view_notices();
                break;
            case 9:
                toggle_dark_mode();
                break;
        }
        if (ch != 0) pause_screen();
    } while (ch != 0);
}

void lecturer_menu(const char *lecturer_id) {
    int ch;
    do {
        clear_screen();
        set_theme();
        printf("==== Lecturer Panel ====\n");
        printf("1. View Profile\n2. View Courses\n3. Enter Marks\n4. Mark Attendance\n5. View Notices\n6. Theme Toggle\n0. Logout\n");
        reset_theme();
        ch = input_int("Enter choice: ", 0, 6);
        switch (ch) {
            case 1:
                search_lecturer();
                break;
            case 2:
                view_courses();
                break;
            case 3:
                enter_marks();
                break;
            case 4:
                mark_attendance();
                break;
            case 5:
                view_notices();
                break;
            case 6:
                toggle_dark_mode();
                break;
        }
        if (ch != 0) pause_screen();
    } while (ch != 0);
}

// =================== MAIN ===================

int main() {
    Role role;
    char user_id[MAX_ID];
    while (1) {
        login_panel(&role, user_id);
        if (role == ROLE_ADMIN)
            admin_menu();
        else if (role == ROLE_STUDENT)
            student_menu(user_id);
        else if (role == ROLE_LECTURER)
            lecturer_menu(user_id);
    }
    return 0;
}

