Consider the following simplified schema that contains information about a bookstore. <pk> represents primary key and <fk> represents foreign key. <pk> and <fk> tags are not part of the attribute name.

```
Books(book_id<pk>, title, author_id<fk>, price, publication_date, genre)
Authors(author_id<pk>, name, country)
Sales(sale_id<pk>, book_id<fk>, sale_date, quantity, customer_id<fk>)
Customers(customer_id<pk>, name, email, join_date)
Reviews(review_id<pk>, book_id<fk>, customer_id<fk>, rating, comment, review_date)
```

## Table creation:

Q) **Write SQL statements to create 'Books' and 'Authors' tables. Specify the most suitable data types and constraints. Write the SQL statements in the order you will execute them.**

```
CREATE TABLE Authors (
  author_id   INT PRIMARY KEY,
  name        VARCHAR(100) NOT NULL,
  country     VARCHAR(60)
);
```

```
CREATE TABLE Books (
  book_id           INT PRIMARY KEY,
  title             VARCHAR(255) NOT NULL,
  author_id         INT NOT NULL,
  price             NUMERIC(10,2) NOT NULL CHECK (price >= 0),
  publication_date  DATE,
  genre             VARCHAR(50),
  FOREIGN KEY (author_id) REFERENCES Authors(author_id)
);
```

## SQL Statements

Q) **Find customers who have written at least one review. The resulting table should have customer_id, name columns.**

```
SELECT DISTINCT c.customer_id, c.name
FROM Customers c
JOIN Reviews r ON r.customer_id = c.customer_id;
```

Q) **For each customer who has reviewed a book find the unique author names associated with the books they reviewed. The resulting table should have customer_id, customer_name, author_name columns.**

```
SELECT DISTINCT
  c.customer_id,
  c.name AS customer_name,
  a.name AS author_name
FROM Customers c
JOIN Reviews r ON r.customer_id = c.customer_id
JOIN Books b    ON b.book_id = r.book_id
JOIN Authors a  ON a.author_id = b.author_id;
```

Q) **Find authors who have books but have not sold any of them. The result should have author_id, author_name columns.**

```
SELECT DISTINCT a.author_id, a.name AS author_name
FROM Authors a
JOIN Books b ON b.author_id = a.author_id
LEFT JOIN Sales s ON s.book_id = b.book_id
WHERE s.book_id IS NULL;
```

Q) **For each author, find customers who have bought and also reviewed at least one of their books. The book that was purchased and the book that was reviewed by a given customer can be different as long as it is from the same author. The result should have author_id, author_name, customer_id, customer_name.**

```
SELECT DISTINCT
  a.author_id,
  a.name AS author_name,
  c.customer_id,
  c.name AS customer_name
FROM Authors a
JOIN Books b1   ON b1.author_id = a.author_id
JOIN Sales s    ON s.book_id = b1.book_id
JOIN Customers c ON c.customer_id = s.customer_id
WHERE EXISTS (
  SELECT 1
  FROM Reviews r
  JOIN Books b2 ON b2.book_id = r.book_id
  WHERE r.customer_id = c.customer_id
    AND b2.author_id = a.author_id
);
```
