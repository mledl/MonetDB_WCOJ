# Query4

## Usage

In shell:
```sh
poetry run python3 gendb/main.py 4 10000 1
```
paramters are in order: query type (4 or 6), table size, number of fan-out anchor (described below).

In Monet:
```
sql>\<go4.sql

sql>select count(*) from r natural join s natural join t natural join u;
+-------+
| %1    |
+=======+
| 13330 |
+-------+
1 tuple
sql:0.821 opt:1.508 run:2445.438 clk:2449.135 ms
```

## Info

The tables and columns are described in the `go4.sql` script and the `tables4` dictionary in the python script. For 1 fan-out anchor we have the analogue of the triangle database. For example, table size 7 and 1 fan-out anchor (`main.py 4 10 1`) we get the following data for table `R`:
```
a,b,c
a0,b0,c0
a0,b0,c1
a0,b0,c2
a0,b1,c0
a0,b2,c0
a1,b0,c0
a2,b0,c0
```

That is, with some abuse of notation `(a0,b0) x {c0,c1,c2}` union `(a0,c0) x {b0,b1,b2}` union `{a0,a1,a2} x (b0,c0)`. Thus for every pair of index 0 (the fan out anchor here) the values fan out to `x = m // 4` tuples. The value x is a normalized version of m to make the data comparable between differeny queries (it can be disabled by the `--no-normalize` parameter.

Note that increasing the fan-out also scales up the relations (this is not normalized) but may increase join processing effort beyond just the increase in input tuples. The example from above with 2 fan-out anchors takes around 6.2sec on my machine for reference (thus factor 2.5 slowdown vs. factor 2 increase in tables).

# Query6
The query is very similar as before but scaled up to more edges and attributed (and lower fractional cover number).

In shell:
```sh
poetry run python3 gendb/main.py 6 10000 1
```

In Monet:
```
sql>\<go6.sql

sql>select count(*) from r natural join s natural join t natural join u natural join v natural join w;
+-------+
| %1    |
+=======+
| 11996 |
+-------+
1 tuple
sql:1.853 opt:2.469 run:6611.932 clk:6618.134 ms
```

# Cover Numbers
For both queries (and the triangle query) the optimal fractional cover number is 1+1/(|V|-1). This is achieved by setting every edge to weight 1/(|V|-1) thus giving |V|/(|V|-1) total weight.

Note that for various reasons, classical methods should also scale only quadratically on these queries. It is therefore not expected that the normal query evaluation becomes significantly worse for the larger query.

# Possible Future Work
If than fan-out changes give interesting results the script could easily be adapted to also produce databases with multiple fan-out anchors for the traingle query.
