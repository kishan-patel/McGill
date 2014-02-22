drop table Series
drop table Games
drop trigger UPDATE_GAMES_WON

create Table Series(id int not NULL, round int, team1 varchar(30), team2 varchar(30), games_won1 int, games_won2 int, primary key(id))
create Table Games(id int not null, game int not null, date date, goals1 int default 0, goals2 int default 0, ot int default 0, primary key(game, id), foreign key(id) references Series)

insert into Series values(1,1,'Montreal Canadiens','Toronto Maple Leafs',4,0)
insert into Series values(2,1,'Chicago Black Hawks','Detroid Red Wings',4,1)
insert into Series values(3,2,'Montreal Canadiens','Chicago Black Hawks',1,0)

insert into Games values(1,1,'2014-01-01',2,1,0)
insert into Games values(1,2,'2014-01-02',6,0,0)
insert into Games values(1,3,'2014-01-03',2,1,0)
insert into Games values(1,4,'2014-01-04',6,5,1)

insert into Games values(2,1,'2014-01-05',6,2,0)
insert into Games values(2,2,'2014-01-06',5,1,0)
insert into Games values(2,3,'2014-01-07',4,0,0)
insert into Games values(2,4,'2014-01-08',0,2,0)
insert into Games values(2,5,'2014-01-09',2,1,1)

insert into Games values(3,1,'2014-01-10',1,0,1)
