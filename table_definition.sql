# user table
# table denifition
create table users( name text primary key, password text, p integer, g integer, t integer, sa integer);
# table populate
insert into USERS ("NAME") values ("AndyNovo");
insert into USERS ("NAME") values ("tongming");
insert into USERS ("NAME") values ("sam");
insert into USERS ("NAME") values ("yifeng");
insert into USERS ("NAME") values ("clara");
insert into USERS ("NAME") values ("god");


# mail table
# table denifition
create table MAIL( mailid integer primary key autoincrement,sender text,receiver text, title text, message text);

# table populate
insert into MAIL ("sender", "receiver", "title", "message") values ("andy", "tongming", "nice to meet you", "hi i am god, who are your");
insert into MAIL ("sender", "receiver", "title", "message") values ("andy", "tongming", "title2", "who are your");
insert into MAIL ("sender", "receiver", "title", "message") values ("tongming", "andy", "title", "message");

	
insert into MAIL ("sender", "receiver", "title", "message") values ("tongming", "andy", "title", "message");
insert into MAIL ("sender", "receiver", "title", "message") values ("god", "tongming", "nice to meet you", "hi i am god, who are your");
insert into MAIL ("sender", "receiver", "title", "message") values ("god", "tongming", "nice to meet you", "hi i am god, who are your");
insert into MAIL ("sender", "receiver", "title", "message") values ("god", "tongming", "nice to meet you", "hi i am god, who are your");
