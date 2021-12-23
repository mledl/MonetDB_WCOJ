import sys
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

def parse_csv_files(filenames):
  data = {}
  data_avg = {}
  query_cnt = 0
  current_run = -1
  max_r = -1
  m = []

  for filename in filenames:
    with open(filename) as f:
      curr_query = ''

      for line in f.readlines():
        line = line.strip()
        subkey = filename.split('/')[-1].split('.')[0]

        if line.startswith('select'):
          query_cnt += 1

          if line not in data:
            data[line] = {}
            data_avg[line] = {}

          if subkey not in data[line]:
            data[line][subkey] = {}
            data_avg[line][subkey] = []

          curr_query = line
        else:
          parts = line.split(';')

          if parts[0] == 'r':
            current_run = int(parts[1])
            data[curr_query][subkey][current_run] = []
			
            if current_run == 2:
              data[curr_query]['m'] = m
              m = []
			
            if current_run > max_r:
              max_r = current_run
          else:
            data[curr_query][subkey][current_run].append(float(parts[1].strip())/1000)

          if current_run == 1 and parts[0] != 'r' and current_run == 1:
            m.append(int(parts[0]))
  
  for key in data:
    for t in data_avg[key]:
      data_avg[key][t] = [sum(x)/max_r for x in zip(*list(data[key][t].values()))]
    data_avg[key]['m'] = data[key]['m']

  return data, data_avg

if __name__ == "__main__":
  query_names = ['go3', 'go4', 'go6']

  # raw data contains the individual runs
  data_raw, data_avg = parse_csv_files(sys.argv[1:])

  # print avg data
  for idx, key in enumerate(data_avg):
    df = pd.DataFrame.from_dict(data_avg[key])
    df_scalar = df.set_index('m')

    ax = df_scalar.plot(title=key)
    ax.set_xlabel('m')
    ax.set_ylabel('runtime [s]')

    fig = ax.get_figure()
    fig.savefig('results/' + query_names[idx] + '.png', bbox_inches='tight')

    df['m^1.5'] = (df['m']**1.5)
    df['m^2'] = (df['m']**2)
    df_log = np.log(df)
    df_log['m^1.5'] = df_log['m^1.5'] - (df_log['m^1.5'].max() - df_log['monetdb_wcoj_optimized'].max())
    df_log['m^2'] = df_log['m^2'] - (df_log['m^2'].max() - df_log['monetdb_classic'].max())
    df_log = df_log.set_index('m')

    ax = df_log.plot(title=key)
    ax.set_xlabel('log(m)')
    ax.set_ylabel('log(runtime [s])')

    fig = ax.get_figure()
    fig.savefig('results/log_' + query_names[idx] + '.png', bbox_inches='tight')
