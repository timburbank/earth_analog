'''
Read potentiometer profiles recorded by pot_profiling.ino and save LUT
'''
import argparse
import csv
import logging

logging.basicConfig(
    format='%(levelname)s %(funcName)s %(lineno)s: %(message)s', 
    level=logging.DEBUG)


def build_lut(profile_path, out_path):
    '''
    Read profile files and calculate a LUT

    Args:
        profile_path (str): Path of profile file
        out_path (str): Path of CSV file to save result
    '''
    ## Save profile data in list with normalized time as key
    profile_data = []
    with open(profile_path) as pro_file:
        reader = csv.reader(pro_file, delimiter='\t')
        for row in reader:
            profile_data.append([int(row[0]), int(row[1])])

    start_time = profile_data[0][0]

    for row in profile_data:
        row[0] = row[0] - start_time

    end_time_norm = profile_data[-1][0]

    ## Remap time to 0-127 scale and make direction low to high on both fields
    if (profile_data[0][1] < profile_data[-1][1]):
        ## normal direction
        for row in profile_data:
            row[0] = row[0] * 127 / end_time_norm
    else:
        ## reversed
        for row in profile_data:
            row[0] = (end_time_norm - row[0]) * 127 / end_time_norm

    ## Save to dict because I'm tired and this seems easier to work with
    ## We're indexed by analogRead() value this time
    profile_dict = {}
    for row in profile_data:
        profile_dict[row[1]] = row[0]

    ## these values we know but aren't recorded by pot_profiling.ini
    profile_dict[0] = 0
    profile_dict[1023] = 127


    ## Fill in missing values
    lut_data = [None] * 1024
    lut_data[0] = 0
    lut_data[1023] = 127

    for i in range(1, 1023): ## 1023 is max value for analogRead()
        if i in profile_dict:
            lut_data[i] = profile_dict[i]
        else:
            ## find nearest neighbor
            for search_range in range(1, 1023):
                if i + search_range in profile_dict:
                    lut_data[i] = profile_dict[i + search_range]
                    break
                elif i - search_range in profile_dict:
                    lut_data[i] = profile_dict[i - search_range]
                    break


    with open(out_path, 'a') as out_file:
        writer = csv.writer(out_file, delimiter='\t')
        writer.writerow(lut_data)


def calculate_average(csv_path):
    '''
    Calculate the average of each column of a CSV file
    '''
    with open(csv_path) as csv_file:
        reader = csv.reader(csv_file, delimiter='\t')
        num_cols = len(reader.next())

    averages = [None] * num_cols

    for col_index in range(0, num_cols):
        col_sum = 0
        num_rows = 0

        with open(csv_path) as csv_file:
            reader = csv.reader(csv_file, delimiter='\t')

            for row in reader:
                col_sum += int(row[col_index])
                num_rows += 1

            col_average = col_sum / num_rows
            averages[col_index] = int(col_average)

    with open(csv_path, 'a') as out_file:
        writer = csv.writer(out_file, delimiter='\t')
        writer.writerow(averages)


def format_c_array(data):
    '''
    Format a list as a C array

    Args:
        data (list)

    Returns:
        (str): C array formatted string
    '''

    result = '{'

    for item in data:
        result += '{}, '.format(item)

    result = result[0:-2] + '}'
    return result




def cli_args():
    parser = argparse.ArgumentParser(description='__doc__')

    parser.add_argument(
        'profile',
        nargs='*',
        help='Profile file, should have a column of milliseconds, ' \
            + 'and a tab separated column of analogRead() values')

    parser.add_argument(
        'outfile',
        help='Path of CSV file to save results')

    return parser.parse_args()


if __name__ == '__main__':
    args = cli_args()
    for profile_path in args.profile:
        build_lut(profile_path, args.outfile)

    calculate_average(args.outfile)

    with open(args.outfile) as csv_file:
        reader = csv.reader(csv_file, delimiter='\t')

        # this is nonsense but I'm tired
        for row in reader:
            last_row = row

    print(format_c_array(last_row))
