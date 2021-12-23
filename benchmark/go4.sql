drop table r;
drop table s;
drop table t;
drop table u;

create table r(a string, b string, c string);
create table s(b string, c string, d string);
create table t(a string, c string, d string);
create table u(a string, b string, d string);

copy into R from '/home/mledl/Desktop/diplomarbeit/benchmark/R.csv' using delimiters ',';
copy into S from '/home/mledl/Desktop/diplomarbeit/benchmark/S.csv' using delimiters ',';
copy into T from '/home/mledl/Desktop/diplomarbeit/benchmark/T.csv' using delimiters ',';
copy into U from '/home/mledl/Desktop/diplomarbeit/benchmark/U.csv' using delimiters ',';
