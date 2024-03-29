#include "database.h"
#include "qsqlerror.h"

DataBase::DataBase(QObject *parent)
    : QObject{parent}
{
    dataBase = new QSqlDatabase();
}

DataBase::~DataBase()
{
    delete dataBase;
}

/*!
 * \brief Метод добавляет БД к экземпляру класса QSqlDataBase
 * \param driver драйвер БД
 * \param nameDB имя БД (Если отсутствует Qt задает имя по умолчанию)
 */
void DataBase::AddDataBase(QString driver, QString nameDB)
{
    *dataBase = QSqlDatabase::addDatabase(driver, nameDB);
}

/*!
 * \brief Метод подключается к БД
 * \param для удобства передаем контейнер с данными необходимыми для подключения
 * \return возвращает тип ошибки
 */
void DataBase::ConnectToDataBase(QVector<QString> data)
{
    dataBase->setHostName(data[hostName]);
    dataBase->setDatabaseName(data[dbName]);
    dataBase->setUserName(data[login]);
    dataBase->setPassword(data[pass]);
    dataBase->setPort(data[port].toInt());

    ///Тут должен быть код ДЗ

    bool status;
    status = dataBase->open();
    if (status){
        tableModel = new QSqlTableModel(this, *dataBase);
        queryModel = new QSqlQueryModel(this);
    }
    emit sig_SendStatusConnection(status);
}
/*!
 * \brief Метод производит отключение от БД
 * \param Имя БД
 */
void DataBase::DisconnectFromDataBase(QString nameDb)
{
    *dataBase = QSqlDatabase::database(nameDb);
    dataBase->close();
    delete tableModel;
    delete queryModel;

}
/*!
 * \brief Метод формирует запрос к БД.
 * \param request - SQL запрос
 * \return
 */
void DataBase::RequestToDB(QTableView* tb_result, const requestType filtr)
{
    ///Тут должен быть код ДЗ

    if (filtr == requestAllFilms) {
        tableModel->setTable("film");
        tableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        tableModel->setHeaderData(1, Qt::Horizontal, tr("Название"));
        tableModel->setHeaderData(2, Qt::Horizontal, tr("Описание"));
        tableModel->select();
        tb_result->setModel(tableModel);

        for (int col = 0; col < tableModel->columnCount(); ++col) {
            if (col != 1 && col != 2) { // Скрыть все столбцы, кроме второго и третьего
                tb_result->setColumnHidden(col, true);
            }
        }
        qDebug() << "Request has been sended";
        qDebug() << "DataBase: " << tableModel->database();
        qDebug() << "Table: " << tableModel->tableName();
    }
    else {
        QString request = "SELECT title, description FROM film f "
                                      "JOIN film_category fc on f.film_id = fc.film_id "
                                      "JOIN category c on c.category_id = fc.category_id ";

        if  (filtr == requestComedy) {
            request = request + " WHERE c.name = 'Comedy'";
        }
        else {
            request = request + " WHERE c.name = 'Horror'";
        }
        queryModel->setQuery(request, *dataBase);
        qDebug() << queryModel->lastError();
        qDebug() << queryModel->query().lastQuery();
        queryModel->setHeaderData(0, Qt::Horizontal, tr("Название"));
        queryModel->setHeaderData(1, Qt::Horizontal, tr("Описание"));
        tb_result->setModel(queryModel);
        for (int col = 0; col < queryModel->columnCount(); ++col) {
            if (col != 0 && col != 1) { // Скрыть все столбцы, кроме второго и третьего
                tb_result->setColumnHidden(col, true);
            }
        }
    }
    tb_result->resizeColumnsToContents();
}

/*!
 * @brief Метод возвращает последнюю ошибку БД
 */
QSqlError DataBase::GetLastError()
{
    return dataBase->lastError();
}
