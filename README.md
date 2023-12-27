# Задание
Необходимо модифицировать приложение, которое было создано в предыдущем задании. Для сущности, отвечающей за хранение клиента (пользователя) необходимо настроить sharding на основе первичного ключа. Все остальные сущности должны храниться в одном экземпляре базы данных.

## Должны выполняться следующие условия:

- Данные должны храниться в СУБД MariaDb;
- Необходимо распределить данные между двумя серверами равномерно по ключевому;
- Sharding должен быть настроен с помощью программного обеспечения ProxySQL;
- Запрос на поиск клиента (пользователя) по маске имени и фамилии должен продолжать работать;
- Программа при старте должна создавать нужные схемы и таблицы во всех инстансах СУБД;

Работы сдаются в виде исходного кода, опубликованного на Github.

В репозитории должен располагаться скрипт для инициализации базы данных и заполнения тестовых значений.

В репозитории должен располагаться файл c описанием API в формате OpenAPI

Репозиторий должен содержать docker-compose файл для запуска приложения и dockerfile-ы для сборки его образов.

В репозитории должно располагаться описание архитектуры проекта

```plantuml
@startuml
!include https://raw.githubusercontent.com/plantuml-stdlib/C4-PlantUML/master/C4_Container.puml

AddElementTag("microService", $shape=EightSidedShape(), $bgColor="CornflowerBlue", $fontColor="white", $legendText="microservice")
AddElementTag("storage", $shape=RoundedBoxShape(), $bgColor="lightSkyBlue", $fontColor="white")

Container(auth_api, "auth_api", "C++", "Сервис авторизаций b поиска пользователей", $tags = "microService")   
Container(messanger_api, "messanger_api", "C++", "Сервис управления публичными чатами и личными сообщениями", $tags = "microservice")
ContainerDb(db_node_1, "База данных", "MariaDB", "Хранение данных о чатах, сообщениях, и пользователях", $tags = "storage")   
ContainerDb(db_node_2, "База данных", "MariaDB", "Хранение данных о пользователях", $tags = "storage")  
ContainerDb(proxysql, "База данных", "ProxySQL", "Хранение данных о чатах, сообщениях, и пользователях", $tags = "storage")  

Rel(auth_api, proxysql, "Получение данных о пользователе", "SQL")

Rel(messanger_api, proxysql, "Получение данных о сообщениях и чатах", "SQL")
Rel(messanger_api, auth_api, "Авторизация пользователй", "HTTP")

Rel(proxysql, db_node_1, "INSERT/SELECT/UPDATE", "SQL")

Rel(proxysql, db_node_2, "INSERT/SELECT/UPDATE", "SQL")

@enduml
```