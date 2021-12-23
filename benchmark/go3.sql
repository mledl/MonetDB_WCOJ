drop table r;
drop table s;
drop table t;

create table r(a string, b string);
create table s(b string, c string);
create table t(a string, c string);

copy into R from '/home/mledl/Desktop/diplomarbeit/benchmark/R.csv' using delimiters ',';
copy into S from '/home/mledl/Desktop/diplomarbeit/benchmark/S.csv' using delimiters ',';
copy into T from '/home/mledl/Desktop/diplomarbeit/benchmark/T.csv' using delimiters ',';
