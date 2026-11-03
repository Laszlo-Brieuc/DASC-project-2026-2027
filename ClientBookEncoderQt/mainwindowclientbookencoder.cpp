#include "mainwindowclientbookencoder.h"
#include "ui_mainwindowclientbookencoder.h"
#include "unistd.h"
#include <QInputDialog>
#include <QMessageBox>
#include <iostream>
using namespace std;

#include "tcp.h"

// Global socket descriptor for client connection
static int sClient = -1;

MainWindowClientBookEncoder::MainWindowClientBookEncoder(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowClientBookEncoder)
{
    ui->setupUi(this);
    ::close(2);

    // Configuration de la table des livres encodés
    ui->tableWidgetEncodedBooks->setColumnCount(9);
    ui->tableWidgetEncodedBooks->setRowCount(0);
    QStringList labelsTableEmployes;
    labelsTableEmployes << "Id" << "Titre" << "Auteur" << "Sujet" << "ISBN" << "Pages" << "Année" << "Prix" << "Stock";
    ui->tableWidgetEncodedBooks->setHorizontalHeaderLabels(labelsTableEmployes);
    ui->tableWidgetEncodedBooks->horizontalHeader()->setVisible(true);
    ui->tableWidgetEncodedBooks->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidgetEncodedBooks->verticalHeader()->setVisible(false);
    ui->tableWidgetEncodedBooks->horizontalHeader()->setStyleSheet("background-color: lightyellow");
    int columnWidths[] = {35, 250, 200, 200, 150, 50, 50, 50, 40};
    for (int col = 0; col < 9; ++col)
        ui->tableWidgetEncodedBooks->setColumnWidth(col, columnWidths[col]);

    this->logoutOk();

    // Clear hardcoded examples
    this->clearTableBooks();
    this->clearComboBoxAuthors();
    this->clearComboBoxSubjects();
}

MainWindowClientBookEncoder::~MainWindowClientBookEncoder() {
    if (sClient != -1) {
        Send(sClient, (char*)"LOGOUT", strlen("LOGOUT"));
        ::close(sClient);
        sClient = -1;
    }
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles Table des livres encodés (ne pas modifier) ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindowClientBookEncoder::addTupleTableBooks(int id,
                                                     string title,
                                                     string author,
                                                     string subject,
                                                     string isbn,
                                                     int pageCount,
                                                     int publishYear,
                                                     float price,
                                                     int stockQuantity)
{
    int nb = ui->tableWidgetEncodedBooks->rowCount();
    nb++;
    ui->tableWidgetEncodedBooks->setRowCount(nb);
    ui->tableWidgetEncodedBooks->setRowHeight(nb-1,10);

    // id
    QTableWidgetItem *item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::number(id));
    ui->tableWidgetEncodedBooks->setItem(nb-1,0,item);

    // title
    item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setText(QString::fromStdString(title));
    ui->tableWidgetEncodedBooks->setItem(nb-1,1,item);

    // author
    item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::fromStdString(author));
    ui->tableWidgetEncodedBooks->setItem(nb-1,2,item);

    // subject
    item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::fromStdString(subject));
    ui->tableWidgetEncodedBooks->setItem(nb-1,3,item);

    // isbn
    item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::fromStdString(isbn));
    ui->tableWidgetEncodedBooks->setItem(nb-1,4,item);

    // pageCount
    item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::number(pageCount));
    ui->tableWidgetEncodedBooks->setItem(nb-1,5,item);

    // publishYear
    item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::number(publishYear));
    ui->tableWidgetEncodedBooks->setItem(nb-1,6,item);

    // price
    item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::number(price));
    ui->tableWidgetEncodedBooks->setItem(nb-1,7,item);

    // stockQuantity
    item = new QTableWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::number(stockQuantity));
    ui->tableWidgetEncodedBooks->setItem(nb-1,8,item);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindowClientBookEncoder::clearTableBooks() {
    ui->tableWidgetEncodedBooks->setRowCount(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles des comboboxes (ne pas modifier) //////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindowClientBookEncoder::addComboBoxAuthors(string author){
    ui->comboBoxAuthors->addItem(QString::fromStdString(author));
}

string MainWindowClientBookEncoder::getSelectionAuthor() const {
    return ui->comboBoxAuthors->currentText().toStdString();
}

void MainWindowClientBookEncoder::clearComboBoxAuthors() {
    ui->comboBoxAuthors->clear();
}

void MainWindowClientBookEncoder::addComboBoxSubjects(string subject){
    ui->comboBoxSubjects->addItem(QString::fromStdString(subject));
}

string MainWindowClientBookEncoder::getSelectionSubject() const {
    return ui->comboBoxSubjects->currentText().toStdString();
}

void MainWindowClientBookEncoder::clearComboBoxSubjects() {
    ui->comboBoxSubjects->clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonction utiles de la fenêtre (ne pas modifier) ////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
string MainWindowClientBookEncoder::getTitle() const {
    return ui->lineEditTitle->text().toStdString();
}

string MainWindowClientBookEncoder::getIsbn() const {
    return ui->lineEditIsbn->text().toStdString();
}

int MainWindowClientBookEncoder::getPageCount() const {
    return ui->spinBoxPageCount->value();
}

float MainWindowClientBookEncoder::getPrice() const {
    return ui->doubleSpinBoxPrice->value();
}

int MainWindowClientBookEncoder::getPublishYear() const {
    return ui->spinBoxPublishYear->value();
}

int MainWindowClientBookEncoder::getStockQuantity() const {
    return ui->spinBoxStockQuantity->value();
}

void MainWindowClientBookEncoder::loginOk() {
    ui->pushButtonClear->setEnabled(true);
    ui->pushButtonAddBook->setEnabled(true);
    ui->pushButtonAddAuthor->setEnabled(true);
    ui->pushButtonAddSubject->setEnabled(true);
    ui->actionLogin->setEnabled(false);
    ui->actionLogout->setEnabled(true);
}

void MainWindowClientBookEncoder::logoutOk() {
    ui->pushButtonClear->setEnabled(false);
    ui->pushButtonAddBook->setEnabled(false);
    ui->pushButtonAddAuthor->setEnabled(false);
    ui->pushButtonAddSubject->setEnabled(false);
    ui->actionLogin->setEnabled(true);
    ui->actionLogout->setEnabled(false);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions permettant d'afficher des boites de dialogue (ne pas modifier) ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindowClientBookEncoder::dialogMessage(const string& title,const string& message) {
   QMessageBox::information(this,QString::fromStdString(title),QString::fromStdString(message));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindowClientBookEncoder::dialogError(const string& title,const string& message) {
   QMessageBox::critical(this,QString::fromStdString(title),QString::fromStdString(message));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
string MainWindowClientBookEncoder::dialogInputText(const string& title,const string& question) {
    return QInputDialog::getText(this,QString::fromStdString(title),QString::fromStdString(question)).toStdString();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MainWindowClientBookEncoder::dialogInputInt(const string& title,const string& question) {
    return QInputDialog::getInt(this,QString::fromStdString(title),QString::fromStdString(question));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions gestion des boutons et items de menu (TO DO) /////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindowClientBookEncoder::on_actionLogin_triggered() {
    string login = this->dialogInputText("Entrée en session","Login ?");
    string password = this->dialogInputText("Entrée en session","Password ?");

    if (login.empty() || password.empty()) return;

    // Connect socket if not already connected
    if (sClient == -1) {
        sClient = ClientSocket((char*)"127.0.0.1", 5000);
        if (sClient == -1) {
            this->dialogError("Erreur Connexion", "Impossible de se connecter au serveur Encodage !");
            return;
        }
    }

    // Prepare and send LOGIN request
    char request[512];
    snprintf(request, sizeof(request), "LOGIN#%s#%s", login.c_str(), password.c_str());
    Send(sClient, request, strlen(request));

    // Read response
    char response[2048];
    int bytes = Receive(sClient, response);
    if (bytes <= 0) {
        this->dialogError("Erreur Réseau", "Le serveur ne répond pas.");
        ::close(sClient);
        sClient = -1;
        return;
    }

    // Parse response
    char *token = strtok(response, "#");
    if (token && strcmp(token, "LOGIN") == 0) {
        char *status = strtok(NULL, "#");
        if (status && strcmp(status, "ok") == 0) {
            this->loginOk();
            this->dialogMessage("Succès", "Connexion réussie !");

            // Automatically load Authors and Subjects
            this->clearComboBoxAuthors();
            this->clearComboBoxSubjects();

            // 1. GET_AUTHORS Parsing Loop
            Send(sClient, (char*)"GET_AUTHORS", strlen("GET_AUTHORS"));
            char authResp[2048];
            if (Receive(sClient, authResp) > 0) {
                char *ptr = strtok(authResp, "#");
                if (ptr && strcmp(ptr, "GET_AUTHORS") == 0) {
                    ptr = strtok(NULL, "#"); // Skip "ok"
                    while ((ptr = strtok(NULL, "#")) != NULL) {
                        int id = 0;
                        char lName[100] = {0}, fName[100] = {0};
                        // Expecting: id;last_name;first_name
                        // Correction to ensure spaced names or names with - don't break it.
                        if (sscanf(ptr, "%d;%99[^;];%99[^\r\n]", &id, lName, fName) >= 2) {
                            string fullName = string(lName) + " " + string(fName);
                            ui->comboBoxAuthors->addItem(QString::fromStdString(fullName), QVariant(id));
                        }
                    }
                }
            }

            // 2. GET_SUBJECTS Parsing Loop
            Send(sClient, (char*)"GET_SUBJECTS", strlen("GET_SUBJECTS"));
            char subjResp[2048];
            if (Receive(sClient, subjResp) > 0) {
                char *ptr = strtok(subjResp, "#");
                if (ptr && strcmp(ptr, "GET_SUBJECTS") == 0) {
                    ptr = strtok(NULL, "#"); // Skip "ok"
                    while ((ptr = strtok(NULL, "#")) != NULL) {
                        int id = 0;
                        char sName[100] = {0};
                        // Expecting: id;name
                        if (sscanf(ptr, "%d;%99[^\r\n#]", &id, sName) == 2) {
                            ui->comboBoxSubjects->addItem(QString::fromStdString(sName), QVariant(id));
                        }
                    }
                }
            }

        } else {
            char *msg = strtok(NULL, "#");
            this->dialogError("Erreur Connexion", msg ? msg : "Identifiants incorrects !");
        }
    }
}

void MainWindowClientBookEncoder::on_actionLogout_triggered() {
    if (sClient != -1) {
        Send(sClient, (char*)"LOGOUT", strlen("LOGOUT"));
        char response[256];
        Receive(sClient, response);
        ::close(sClient);
        sClient = -1;
    }
    this->clearComboBoxAuthors();
    this->clearComboBoxSubjects();
    this->clearTableBooks();
    this->logoutOk();
    this->dialogMessage("Déconnexion", "Session fermée.");
}

void MainWindowClientBookEncoder::on_pushButtonAddAuthor_clicked() {
    string lastName = this->dialogInputText("Nouvel auteur", "Nom ?");
    string firstName = this->dialogInputText("Nouvel auteur", "Prénom ?");

    if (lastName.empty() || firstName.empty()) return;

    char request[512];
    snprintf(request, sizeof(request), "ADD_AUTHOR#%s#%s", lastName.c_str(), firstName.c_str());
    Send(sClient, request, strlen(request));

    char response[512];
    if (Receive(sClient, response) > 0) {
        char *ptr = strtok(response, "#");
        if (ptr && strcmp(ptr, "ADD_AUTHOR") == 0) {
            char *status = strtok(NULL, "#");
            char *val = strtok(NULL, "#");
            if (status && strcmp(status, "ok") == 0 && val) {
                int newId = atoi(val);
                if (newId != -1) {
                    string fullName = lastName + " " + firstName;
                    ui->comboBoxAuthors->addItem(QString::fromStdString(fullName), QVariant(newId));
                    this->dialogMessage("Succès", "Auteur ajouté avec succès !");
                } else {
                    this->dialogError("Erreur", "Auteur déjà existant dans la base de données !");
                }
            }
        }
    }
}

void MainWindowClientBookEncoder::on_pushButtonAddSubject_clicked() {
    string name = this->dialogInputText("Nouveau sujet", "Nom ?");
    if (name.empty()) return;

    char request[512];
    snprintf(request, sizeof(request), "ADD_SUBJECT#%s", name.c_str());
    Send(sClient, request, strlen(request));

    char response[512];
    if (Receive(sClient, response) > 0) {
        char *ptr = strtok(response, "#");
        if (ptr && strcmp(ptr, "ADD_SUBJECT") == 0) {
            char *status = strtok(NULL, "#");
            char *val = strtok(NULL, "#");
            if (status && strcmp(status, "ok") == 0 && val) {
                int newId = atoi(val);
                if (newId != -1) {
                    ui->comboBoxSubjects->addItem(QString::fromStdString(name), QVariant(newId));
                    this->dialogMessage("Succès", "Sujet ajouté avec succès !");
                } else {
                    this->dialogError("Erreur", "Sujet déjà existant dans la base de données !");
                }
            }
        }
    }
}

void MainWindowClientBookEncoder::on_pushButtonAddBook_clicked() {
    string title = this->getTitle();
    string isbn = this->getIsbn();
    int pageCount = this->getPageCount();
    float price = this->getPrice();
    int publishYear = this->getPublishYear();
    int stock = this->getStockQuantity();

    if (title.empty() || isbn.empty()) {
        this->dialogError("Erreur Champs", "Veuillez remplir le titre et l'ISBN.");
        return;
    }

    int authorId = ui->comboBoxAuthors->currentData().toInt();
    int subjectId = ui->comboBoxSubjects->currentData().toInt();

    if (authorId <= 0 || subjectId <= 0) {
        this->dialogError("Erreur Sélection", "Veuillez sélectionner un auteur et un sujet valides.");
        return;
    }

    // --- FIX FOR LOCALE COMMA ISSUE (2,50 -> 2.50) ---
    char priceStr[32];
    snprintf(priceStr, sizeof(priceStr), "%.2f", price);
    for (int i = 0; priceStr[i] != '\0'; i++) {
        if (priceStr[i] == ',') priceStr[i] = '.';
    }
    // ------------------------------------------------

    char request[1024];
    snprintf(request, sizeof(request), 
             "ADD_BOOK#%d#%d#%s#%s#%d#%d#%s#%d", 
             authorId, subjectId, title.c_str(), isbn.c_str(), 
             pageCount, stock, priceStr, publishYear);

    Send(sClient, request, strlen(request));

    char response[512];
    if (Receive(sClient, response) > 0) {
        char *ptr = strtok(response, "#");
        if (ptr && strcmp(ptr, "ADD_BOOK") == 0) {
            char *status = strtok(NULL, "#");
            char *val = strtok(NULL, "#");
            if (status && strcmp(status, "ok") == 0 && val) {
                int newBookId = atoi(val);
                if (newBookId != -1) {
                    this->addTupleTableBooks(newBookId, title, 
                                             this->getSelectionAuthor(), 
                                             this->getSelectionSubject(), 
                                             isbn, pageCount, publishYear, price, stock);
                    this->dialogMessage("Succès", "Livre ajouté à la base de données !");
                } else {
                    this->dialogError("Erreur", "Impossible d'ajouter le livre en base de données.");
                }
            }
        }
    }
}

void MainWindowClientBookEncoder::on_pushButtonClear_clicked() {
    ui->lineEditTitle->clear();
    ui->lineEditIsbn->clear();
    ui->spinBoxPageCount->setValue(0);
    ui->doubleSpinBoxPrice->setValue(0);
    ui->spinBoxPublishYear->setValue(0);
    ui->spinBoxStockQuantity->setValue(0);
}

void MainWindowClientBookEncoder::on_actionQuitter_triggered(){
    if (sClient != -1) {
        Send(sClient, (char*)"LOGOUT", strlen("LOGOUT"));
        ::close(sClient);
        sClient = -1;
    }
    QApplication::exit(0);
}