#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_STUDENTS 1000
#define SUBJECTS 5
#define ADMIN_PASS "admin123"
#define STUDENT_PASS_DEFAULT "student"
#define WORKING_DAYS 22 // per month

typedef struct {
    char id[10];
    char name[50];
    char dept[30];
    int marks[SUBJECTS];
    float avg, gpa;
    char grade;
    int attendance; // number of presents in a month
    char password[20];
} Student;

Student students[MAX_STUDENTS];
int student_count = 0;

// Utility Functions
void pause() { printf("\nPress Enter to continue..."); getchar(); getchar(); }
void clear() { system("cls||clear"); }

// Data Handling
void load_students() {
    FILE *fp = fopen("students.dat", "rb");
    if (fp) {
        fread(&student_count, sizeof(int), 1, fp);
        fread(students, sizeof(Student), student_count, fp);
        fclose(fp);
    }
}

void save_students() {
    FILE *fp = fopen("students.dat", "wb");
    if (fp) {
        fwrite(&student_count, sizeof(int), 1, fp);
        fwrite(students, sizeof(Student), student_count, fp);
        fclose(fp);
    }
}

// GPA & Grade Calculation
float calc_gpa(int marks[]) {
    float sum = 0;
    for (int i = 0; i < SUBJECTS; i++)
        sum += marks[i];
    return (sum / SUBJECTS) / 25.0; // GPA out of 4
}

char calc_grade(float gpa) {
    if (gpa >= 3.7) return 'A';
    if (gpa >= 3.0) return 'B';
    if (gpa >= 2.0) return 'C';
    if (gpa >= 1.0) return 'D';
    return 'F';
}

// Find student by ID
int find_student(char *id) {
    for (int i = 0; i < student_count; i++)
        if (strcmp(students[i].id, id) == 0)
            return i;
    return -1;
}

// Login Logger
void log_login(const char *user, const char *id) {
    FILE *fp = fopen("login_log.txt", "a");
    if (fp) {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d] %s login: %s\n",
            tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec, user, id);
        fclose(fp);
    }
}

// Admin Panel Functions
void admin_add_student() {
    Student s;
    printf("Enter Student ID: "); scanf("%s", s.id);
    if (find_student(s.id) != -1) { printf("ID already exists!\n"); return; }
    printf("Enter Name: "); scanf(" %[^\n]", s.name);
    printf("Enter Department: "); scanf(" %[^\n]", s.dept);
    for (int i = 0; i < SUBJECTS; i++) s.marks[i] = 0;
    s.avg = 0; s.gpa = 0; s.grade = 'F'; s.attendance = 0;
    strcpy(s.password, STUDENT_PASS_DEFAULT);
    students[student_count++] = s;
    save_students();
    printf("Student added!\n");
}

void admin_edit_student() {
    char id[10];
    printf("Enter Student ID to edit: "); scanf("%s", id);
    int idx = find_student(id);
    if (idx == -1) { printf("Not found!\n"); return; }
    printf("Edit Name [%s]: ", students[idx].name);
    scanf(" %[^\n]", students[idx].name);
    printf("Edit Dept [%s]: ", students[idx].dept);
    scanf(" %[^\n]", students[idx].dept);
    save_students();
    printf("Updated!\n");
}

void admin_delete_student() {
    char id[10];
    printf("Enter Student ID to delete: "); scanf("%s", id);
    int idx = find_student(id);
    if (idx == -1) { printf("Not found!\n"); return; }
    for (int i = idx; i < student_count-1; i++)
        students[i] = students[i+1];
    student_count--;
    save_students();
    printf("Deleted!\n");
}

void admin_add_update_marks() {
    char id[10];
    printf("Enter Student ID: "); scanf("%s", id);
    int idx = find_student(id);
    if (idx == -1) { printf("Not found!\n"); return; }
    printf("Enter marks for 5 subjects (0-100):\n");
    for (int i = 0; i < SUBJECTS; i++) {
        printf("Subject %d: ", i+1);
        scanf("%d", &students[idx].marks[i]);
    }
    students[idx].gpa = calc_gpa(students[idx].marks);
    students[idx].avg = students[idx].gpa * 25;
    students[idx].grade = calc_grade(students[idx].gpa);
    save_students();
    printf("Marks updated!\n");
}

void admin_view_students() {
    printf("ID\tName\t\tDept\t\tGPA\tGrade\tAttendance\n");
    for (int i = 0; i < student_count; i++) {
        printf("%s\t%s\t%s\t%.2f\t%c\t%d/%d\n",
            students[i].id, students[i].name, students[i].dept,
            students[i].gpa, students[i].grade, students[i].attendance, WORKING_DAYS);
    }
}

void admin_mark_attendance() {
    char id[10];
    printf("Enter Student ID: "); scanf("%s", id);
    int idx = find_student(id);
    if (idx == -1) { printf("Not found!\n"); return; }
    printf("Enter presents this month (max %d): ", WORKING_DAYS);
    scanf("%d", &students[idx].attendance);
    save_students();
    printf("Attendance updated!\n");
}

void admin_generate_ranklist() {
    // Sort students by GPA descending
    Student temp;
    for (int i = 0; i < student_count-1; i++) {
        for (int j = i+1; j < student_count; j++) {
            if (students[j].gpa > students[i].gpa) {
                temp = students[i]; students[i] = students[j]; students[j] = temp;
            }
        }
    }
    printf("Rank\tID\tName\tGPA\n");
    for (int i = 0; i < student_count; i++)
        printf("%d\t%s\t%s\t%.2f\n", i+1, students[i].id, students[i].name, students[i].gpa);
}

void admin_subject_topper() {
    int subj;
    printf("Enter subject number (1-5): "); scanf("%d", &subj);
    if (subj < 1 || subj > 5) { printf("Invalid!\n"); return; }
    int max = -1, idx = -1;
    for (int i = 0; i < student_count; i++) {
        if (students[i].marks[subj-1] > max) {
            max = students[i].marks[subj-1];
            idx = i;
        }
    }
    if (idx != -1)
        printf("Topper: %s (%s) - %d marks\n", students[idx].name, students[idx].id, max);
    else
        printf("No data.\n");
}

void admin_generate_transcript() {
    char id[10];
    printf("Enter Student ID: "); scanf("%s", id);
    int idx = find_student(id);
    if (idx == -1) { printf("Not found!\n"); return; }
    char fname[30];
    sprintf(fname, "transcript_%s.txt", students[idx].id);
    FILE *fp = fopen(fname, "w");
    if (!fp) { printf("File error!\n"); return; }
    fprintf(fp, "Transcript for %s (%s)\n", students[idx].name, students[idx].id);
    fprintf(fp, "Department: %s\n", students[idx].dept);
    fprintf(fp, "Subject Marks: ");
    for (int i = 0; i < SUBJECTS; i++)
        fprintf(fp, "%d ", students[idx].marks[i]);
    fprintf(fp, "\nGPA: %.2f\nGrade: %c\n", students[idx].gpa, students[idx].grade);
    float perc = (students[idx].attendance * 100.0) / WORKING_DAYS;
    fprintf(fp, "Attendance: %d/%d (%.1f%%)\n", students[idx].attendance, WORKING_DAYS, perc);
    if (students[idx].gpa > 3.5 && perc > 80)
        fprintf(fp, "Remarks: Eligible for Merit Scholarship\n");
    else
        fprintf(fp, "Remarks: -\n");
    fclose(fp);
    printf("Transcript exported to %s\n", fname);
}

void admin_view_feedback() {
    FILE *fp = fopen("feedback.txt", "r");
    if (!fp) { printf("No feedbacks yet.\n"); return; }
    char line[256];
    printf("Feedbacks:\n");
    while (fgets(line, sizeof(line), fp))
        printf("%s", line);
    fclose(fp);
}

// Student Panel Functions
int student_login() {
    char id[10], pass[20];
    printf("Enter Student ID: "); scanf("%s", id);
    int idx = find_student(id);
    if (idx == -1) { printf("Not found!\n"); return -1; }
    printf("Enter Password: "); scanf("%s", pass);
    if (strcmp(pass, students[idx].password) != 0) { printf("Wrong password!\n"); return -1; }
    log_login("Student", id);
    return idx;
}

void student_view_details(int idx) {
    printf("ID: %s\nName: %s\nDept: %s\n", students[idx].id, students[idx].name, students[idx].dept);
}

void student_change_password(int idx) {
    char pass[20];
    printf("Enter new password: "); scanf("%s", pass);
    strcpy(students[idx].password, pass);
    save_students();
    printf("Password changed!\n");
}

void student_view_results(int idx) {
    printf("Subject Marks: ");
    for (int i = 0; i < SUBJECTS; i++)
        printf("%d ", students[idx].marks[i]);
    printf("\nAverage: %.2f\nGPA: %.2f\nGrade: %c\n", students[idx].avg, students[idx].gpa, students[idx].grade);
    // Rank calculation
    int rank = 1;
    for (int i = 0; i < student_count; i++)
        if (students[i].gpa > students[idx].gpa)
            rank++;
    printf("Class Rank: %d/%d\n", rank, student_count);
}

void student_view_attendance(int idx) {
    float perc = (students[idx].attendance * 100.0) / WORKING_DAYS;
    printf("Attendance: %d/%d (%.1f%%)\n", students[idx].attendance, WORKING_DAYS, perc);
    if (perc < 75)
        printf("WARNING: Attendance below 75%%!\n");
}

void student_progress_chart(int idx) {
    printf("Progress Chart (Marks):\n");
    for (int i = 0; i < SUBJECTS; i++) {
        printf("Subject %d: ", i+1);
        int bars = students[idx].marks[i] / 5;
        for (int j = 0; j < bars; j++) printf("|");
        printf(" (%d)\n", students[idx].marks[i]);
    }
}

void student_scholarship_eligibility(int idx) {
    float perc = (students[idx].attendance * 100.0) / WORKING_DAYS;
    if (students[idx].gpa > 3.5 && perc > 80)
        printf("Eligible for Merit Scholarship!\n");
    else
        printf("Not eligible for Merit Scholarship.\n");
}

void student_give_feedback(int idx) {
    FILE *fp = fopen("feedback.txt", "a");
    if (!fp) { printf("File error!\n"); return; }
    char feedback[200];
    printf("Enter feedback (max 199 chars): ");
    getchar(); // flush newline
    fgets(feedback, sizeof(feedback), stdin);
    fprintf(fp, "From %s: %s", students[idx].id, feedback);
    fclose(fp);
    printf("Feedback submitted!\n");
}

// Admin Panel Menu
void admin_panel() {
    char pass[20];
    printf("Admin Password: "); scanf("%s", pass);
    if (strcmp(pass, ADMIN_PASS) != 0) { printf("Wrong password!\n"); pause(); return; }
    log_login("Admin", "admin");
    int ch;
    do {
        clear();
        printf("=== Admin Panel ===\n");
        printf("1. Add Student\n2. Edit Student\n3. Delete Student\n4. Add/Update Marks\n");
        printf("5. View Students\n6. Mark Attendance\n7. Generate Ranklist\n8. Subject-wise Topper\n");
        printf("9. Generate Transcript\n10. View Feedback\n0. Logout\nChoice: ");
        scanf("%d", &ch);
        switch (ch) {
            case 1: admin_add_student(); break;
            case 2: admin_edit_student(); break;
            case 3: admin_delete_student(); break;
            case 4: admin_add_update_marks(); break;
            case 5: admin_view_students(); break;
            case 6: admin_mark_attendance(); break;
            case 7: admin_generate_ranklist(); break;
            case 8: admin_subject_topper(); break;
            case 9: admin_generate_transcript(); break;
            case 10: admin_view_feedback(); break;
            case 0: printf("Logging out...\n"); break;
            default: printf("Invalid!\n");
        }
        if (ch != 0) pause();
    } while (ch != 0);
}

// Student Panel Menu
void student_panel() {
    int idx = student_login();
    if (idx == -1) { pause(); return; }
    int ch;
    do {
        clear();
        printf("=== Student Panel (%s) ===\n", students[idx].name);
        printf("1. View Personal Details\n2. Change Password\n3. View Results\n4. View Attendance\n");
        printf("5. Progress Chart\n6. Scholarship Eligibility\n7. Give Feedback\n0. Logout\nChoice: ");
        scanf("%d", &ch);
        switch (ch) {
            case 1: student_view_details(idx); break;
            case 2: student_change_password(idx); break;
            case 3: student_view_results(idx); break;
            case 4: student_view_attendance(idx); break;
            case 5: student_progress_chart(idx); break;
            case 6: student_scholarship_eligibility(idx); break;
            case 7: student_give_feedback(idx); break;
            case 0: printf("Logging out...\n"); break;
            default: printf("Invalid!\n");
        }
        if (ch != 0) pause();
    } while (ch != 0);
}

// Main Menu
int main() {
    load_students();
    int ch;
    do {
        clear();
        printf("=== Student Management System ===\n");
        printf("1. Admin Panel\n2. Student Panel\n0. Exit\nChoice: ");
        scanf("%d", &ch);
        switch (ch) {
            case 1: admin_panel(); break;
            case 2: student_panel(); break;
            case 0: printf("Exiting...\n"); break;
            default: printf("Invalid!\n"); pause();
        }
    } while (ch != 0);
    save_students();
    return 0;
}
