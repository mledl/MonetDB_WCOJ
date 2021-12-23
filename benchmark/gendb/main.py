"""
  Usage:
    ./main (3|4|6) <m> <n> [--no-normalize]
    ./main -h | --help

  Options:
    -h --help  Show this screen.
    <m>  Fan-out for every anchor element.
    <n>  Number of anchord elements.
    --no-normalize  Do not normalize fan-out.
"""
import docopt


def fanned_out_tpls_from_id(cols, j, m):
    out = []
    for i in range(m):
        for x in cols:
            row = []
            for c in cols:
                if x == c:
                    val = '{}{}'.format(c, i)
                else:
                    val = '{}{}'.format(c, j)
                row.append(val)
            rows = ','.join(row)
            out.append(rows)
    return list(set(out))

def gen_tuples(cols, m, n):
    out = []
    for j in range(n):
        out += fanned_out_tpls_from_id(cols, j, m)
    return list(set(out))



if __name__ == "__main__":
    args = docopt.docopt(__doc__)
    
    tables3 = {'R': ['a', 'b'],
               'S': ['b', 'c'],
               'T': ['a', 'c']}
               
    tables4 = {'R': ['a', 'b', 'c'],
               'S': ['b', 'c', 'd'],
               'T': ['a', 'c', 'd'],
               'U': ['a', 'b', 'd']}

    tables6 = {'R': ['a', 'b', 'c', 'd', 'e'     ],
               'S': [     'b', 'c', 'd', 'e', 'f'],
               'T': ['a',      'c', 'd', 'e', 'f'],
               'U': ['a', 'b',      'd', 'e', 'f'],
               'V': ['a', 'b', 'c',      'e', 'f'],
               'W': ['a', 'b', 'c', 'd',      'f']}

    print(args)
    if args['3']:
        tables = tables3
    elif args['4']:
        tables = tables4
    elif args['6']:
        tables = tables6
    m = int(args['<m>'])
    n = int(args['<n>'])
    
    for tbl, cols in tables.items():
        normalized_m = m // (len(cols))
            
        if args['--no-normalize']:
            normalized_m = m
        t = gen_tuples(cols, normalized_m, n)

        fname = '{}.csv'.format(tbl)
        with open(fname, 'w') as f:
            print(','.join(cols), file=f)
            for row in t:
                print(row, file=f)
