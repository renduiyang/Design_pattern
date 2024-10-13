# QMYSQL笔记

## 1.QMYSQL操作

QMYSQL的连接方式

Qt中有有自己的数据库封装,其中主要是是借助<QtSql>头文件,整体流程大致如下

1. 首先选择数据库方式,这里使用addDatabase()函数选择QMYSQL--也就是mysql数据库
   QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
2. 然后填入数据库连接时的相关参数
   ```c++
    void setDatabaseName(const QString& name); // 数据库名称 (选择对应的database)
    void setUserName(const QString& name); // 用户名
    void setPassword(const QString& password); // 密码
    void setHostName(const QString& host);// 主机名 (数据库所在ip地址)
    void setPort(int p);// 端口号(一般为3306)
    void setConnectOptions(const QString& options = QString());// 连接选项(一般为空)
   ```
3. 创建语句对象,qt中语句对象都是QSqlQuery
   ```c++
    // 创建一个 QSqlQuery 对象
    QSqlQuery query(db);
   // 这里就相当去创建了对于这个数据库的连接对象  QtSql中数据库连接对象都是QSqlQuery
   ```
4. 执行sql语句,使用exec方法执行
   ```c++
   //也可以使用prepare方法 query.prepare("select * from users;")  后续只需要使用query.exec()即可 
   // prepare属于预编译sql语句,提高执行效率 减少解析时间
   if (query.exec("select * from users;")){}
   ```
5. 补充几个常用接口函数
   ```c++
   //qt查询到结果之后,针对这条结果可以使用QSqlQuery中的value函数
   if (query.exec("select * from users;")) {
        qDebug() << query.record().count(); // 返回查询到的字段数
        qDebug() << query.size(); // 返回查询到的结果数(行数)
        for (int i = 0; i < query.record().count(); ++i) {
            //返回字段名
            qDebug() << query.record().fieldName(i);
        }
        //next():移动到查询结果的下一行。 (常与while配合使用)
        //value():获取当前行的字段值。
        while (query.next()) {
            qDebug() << "------------";
            // 知道查询到的每条结果的字段名后就可以用for循环输出
            for (int i = 0; i < query.record().count(); ++i) {
                //query相当于查询到的一条结果,value可以获取到每条结果的列值
                qDebug() << query.value(i).toString();
            }
        }
    }
   // 这里使用query.record()函数得到 QSqlRecord 对象,针对查询结果的一些属性需要借助QSqlRecord
   ```
   ```text
   int count() const; //返回记录中字段数量(也就是数据库表中有多少列)
   bool isEmpty() const; // 判断记录是否为空
   QString fieldName(int i) const; // 根据输入的索引得到字段信息(常与count()配合使用 拿到数据库字段名称--也就是每一列名称)
   ```


