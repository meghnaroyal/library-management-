#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct Book
{
    char title[100];
    char bcode[20];
    int quantity;
    int lended;
    struct Book *next;
};

struct Reader
{
    char srn[20];
    char name[100];
    struct Reader *next;
};

struct Lended
{
    char bcode[20];
    char srn[20];
    char due_date[20];
    struct Lended *next;
};

typedef struct Book Book;
typedef struct Reader Reader;
typedef struct Lended Lended;

Book *bookList = NULL;
Reader *readerList = NULL;
Lended *lendedList = NULL;

void loadBooksData()
{
    FILE *file = fopen("books.csv", "r");
    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        char title[100], bcode[20];
        int quantity, lended;
        sscanf(line, "%[^,],%[^,],%d,%d", title, bcode, &quantity, &lended);

        Book *newBook = (Book *)malloc(sizeof(Book));
        strcpy(newBook->title, title);
        strcpy(newBook->bcode, bcode);
        newBook->quantity = quantity;
        newBook->lended = lended;
        newBook->next = bookList;
        bookList = newBook;
    }

    fclose(file);
}

void loadReadersData()
{
    FILE *file = fopen("readers.csv", "r");
    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        char srn[20], name[100];
        sscanf(line, "%[^,],%[^\n]", srn, name);

        Reader *newReader = (Reader *)malloc(sizeof(Reader));
        strcpy(newReader->srn, srn);
        strcpy(newReader->name, name);
        newReader->next = readerList;
        readerList = newReader;
    }

    fclose(file);
}

void loadLendedData()
{
    FILE *file = fopen("lended.csv", "r");
    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        char bcode[20], srn[20], due_date[20];
        sscanf(line, "%[^,],%[^,],%s", bcode, srn, due_date);

        Lended *newLended = (Lended *)malloc(sizeof(Lended));
        strcpy(newLended->bcode, bcode);
        strcpy(newLended->srn, srn);
        strcpy(newLended->due_date, due_date);
        newLended->next = lendedList;
        lendedList = newLended;
    }

    fclose(file);
}

void saveBooksData()
{
    FILE *file = fopen("books.csv", "w");
    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    Book *temp = bookList;
    while (temp != NULL)
    {
        fprintf(file, "%s,%s,%d,%d\n", temp->title, temp->bcode, temp->quantity, temp->lended);
        temp = temp->next;
    }

    fclose(file);
}

void saveReadersData()
{
    FILE *file = fopen("readers.csv", "w");
    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    Reader *temp = readerList;
    while (temp != NULL)
    {
        fprintf(file, "%s,%s\n", temp->srn, temp->name);
        temp = temp->next;
    }

    fclose(file);
}

void saveLendedData()
{
    FILE *file = fopen("lended.csv", "w");
    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    Lended *temp = lendedList;
    while (temp != NULL)
    {
        fprintf(file, "%s,%s,%s\n", temp->bcode, temp->srn, temp->due_date);
        temp = temp->next;
    }

    fclose(file);
}

void displayBooks()
{
    Book *temp = bookList;
    while (temp != NULL)
    {
        printf("Title: %s, Code: %s, Quantity: %d, Lended: %d\n", temp->title, temp->bcode, temp->quantity, temp->lended);
        temp = temp->next;
    }
}

void displayReaders()
{
    Reader *temp = readerList;
    while (temp != NULL)
    {
        printf("SRN: %s, Name: %s\n", temp->srn, temp->name);
        temp = temp->next;
    }
}

void displayLended()
{
    Lended *temp = lendedList;
    while (temp != NULL)
    {
        printf("Book Code: %s, SRN: %s, Due Date: %s\n", temp->bcode, temp->srn, temp->due_date);
        temp = temp->next;
    }
}

void addBook(char title[], char bcode[], int quantity)
{
    Book *newBook = (Book *)malloc(sizeof(Book));
    strcpy(newBook->title, title);
    strcpy(newBook->bcode, bcode);
    newBook->quantity = quantity;
    newBook->lended = 0;
    newBook->next = bookList;
    bookList = newBook;
}

void issueBook(char bcode[], char srn[])
{
    Book *book = bookList;
    while (book != NULL && strcmp(book->bcode, bcode) != 0)
    {
        book = book->next;
    }

    Reader *reader = readerList;
    while (reader != NULL && strcmp(reader->srn, srn) != 0)
    {
        reader = reader->next;
    }

    if (book == NULL)
    {
        printf("Book with the given code not found.\n");
        return;
    }

    if (reader == NULL)
    {
        printf("Reader with the given SRN not found.\n");
        return;
    }

    if (book->quantity <= book->lended)
    {
        printf("No available copies of the book for lending.\n");
        return;
    }

    book->lended++;
    book->quantity--;
    Lended *newLended = (Lended *)malloc(sizeof(Lended));
    strcpy(newLended->bcode, book->bcode);
    strcpy(newLended->srn, reader->srn);

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    tm_info->tm_min += 5;
    mktime(tm_info);
    strftime(newLended->due_date, sizeof(newLended->due_date), "%Y-%m-%d %H:%M:%S", tm_info);

    newLended->next = lendedList;
    lendedList = newLended;

    saveLendedData();
    saveBooksData();

    printf("Book successfully issued to %s. Due date: %s\n", reader->name, newLended->due_date);
}

void imposeFine(char due_date[])
{
    time_t t = time(NULL);
    struct tm tm_info = *localtime(&t);
    char current_date[20];
    strftime(current_date, sizeof(current_date), "%Y-%m-%d %H:%M:%S", &tm_info);

    int due_year, due_month, due_day, due_hour, due_min, due_sec;
    int current_year, current_month, current_day, current_hour, current_min, current_sec;
    sscanf(due_date, "%d-%d-%d %d:%d:%d", &due_year, &due_month, &due_day, &due_hour, &due_min, &due_sec);
    sscanf(current_date, "%d-%d-%d %d:%d:%d", &current_year, &current_month, &current_day, &current_hour, &current_min, &current_sec);

    int seconds_diff = (current_year - due_year) * 365 * 24 * 60 * 60 +
                       (current_month - due_month) * 30 * 24 * 60 * 60 +
                       (current_day - due_day) * 24 * 60 * 60 +
                       (current_hour - due_hour) * 60 * 60 +
                       (current_min - due_min) * 60 +
                       (current_sec - due_sec);

    if (seconds_diff > 300)
    {
        int fine = (seconds_diff - 300) / 60 * 0.0005;
        printf("Fine imposed: %d\n", fine);
    }
    else
    {
        printf("Book is not overdue.\n");
    }
}

void returnBook(char bcode[], char srn[])
{
    Lended *lended = lendedList;
    Lended *prev = NULL;
    while (lended != NULL && (strcmp(lended->bcode, bcode) != 0 || strcmp(lended->srn, srn) != 0))
    {
        prev = lended;
        lended = lended->next;
    }

    if (lended == NULL)
    {
        printf("No matching lended record found for the given book code and SRN.\n");
        return;
    }

    if (prev == NULL)
    {
        lendedList = lended->next;
    }
    else
    {
        prev->next = lended->next;
    }
    free(lended);

    Book *book = bookList;
    while (book != NULL && strcmp(book->bcode, bcode) != 0)
    {
        book = book->next;
    }

    if (book != NULL && book->lended > 0)
    {
        book->lended--;

        book->quantity++;
        saveLendedData();
        saveBooksData();

        printf("Book successfully returned.\n");

        time_t now = time(NULL);
        struct tm *tm_info = localtime(&now);
        char return_time[20];
        strftime(return_time, sizeof(return_time), "%Y-%m-%d %H:%M:%S", tm_info);

        if (strcmp(return_time, lended->due_date) > 0)
        {
            imposeFine(lended->due_date);
        }
    }
    else
    {
        printf("Book not found or invalid lended count.\n");
    }
}

void addReader(char srn[], char name[])
{
    Reader *newReader = (Reader *)malloc(sizeof(Reader));
    strcpy(newReader->srn, srn);
    strcpy(newReader->name, name);
    newReader->next = readerList;
    readerList = newReader;
}

int main()
{
    loadBooksData();
    loadReadersData();
    loadLendedData();

    int choice;
    char bcode[20], srn[20], title[100], name[100];
    int quantity;

    do
    {
        printf("\nLibrary Management System Menu:\n");
        printf("1. Display Books\n");
        printf("2. Display Readers\n");
        printf("3. Display Lended Books\n");
        printf("4. Add Book\n");
        printf("5. Issue Book\n");
        printf("6. Return Book\n");
        printf("7. Add Reader\n");
        printf("8. Quit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            displayBooks();
            break;
        case 2:
            displayReaders();
            break;
        case 3:
            displayLended();
            break;
        case 4:
            printf("Enter Book Title: ");
            scanf("%s", title);
            printf("Enter Book Code: ");
            scanf("%s", bcode);
            printf("Enter Quantity: ");
            scanf("%d", &quantity);
            addBook(title, bcode, quantity);
            break;
        case 5:
            printf("Enter Book Code: ");
            scanf("%s", bcode);
            printf("Enter Student Roll Number: ");
            scanf("%s", srn);
            issueBook(bcode, srn);
            break;
        case 6:
            printf("Enter Book Code: ");
            scanf("%s", bcode);
            printf("Enter Student Roll Number: ");
            scanf("%s", srn);
            returnBook(bcode, srn);

            break;
        case 8:
            printf("Exiting...\n");
            break;
        case 7:
            printf("Enter Student srn: ");
            scanf("%s", srn);
            printf("Enter Reader Name: ");
            scanf("%s", name);
            addReader(srn, name);
            break;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 8);

    saveBooksData();
    saveReadersData();
    saveLendedData();

    return 0;
}
