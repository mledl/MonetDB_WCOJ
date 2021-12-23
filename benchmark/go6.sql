drop table r;
drop table s;
drop table t;
drop table u;
drop table v;
drop table w;

create table r(a string, b string, c string, d string, e string);
create table s(b string, c string, d string, e string, f string);
create table t(a string, c string, d string, e string, f string);
create table u(a string, b string, d string, e string, f string);
create table v(a string, b string, c string, e string, f string);
create table w(a string, b string, c string, d string, f string);

copy into R from '/home/mledl/Desktop/diplomarbeit/benchmark/R.csv' using delimiters ',';
copy into S from '/home/mledl/Desktop/diplomarbeit/benchmark/S.csv' using delimiters ',';
copy into T from '/home/mledl/Desktop/diplomarbeit/benchmark/T.csv' using delimiters ',';
copy into U from '/home/mledl/Desktop/diplomarbeit/benchmark/U.csv' using delimiters ',';
copy into V from '/home/mledl/Desktop/diplomarbeit/benchmark/V.csv' using delimiters ',';
copy into W from '/home/mledl/Desktop/diplomarbeit/benchmark/W.csv' using delimiters ',';
