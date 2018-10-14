'''
Read potentiometer profiles recorded by pot_profiling.ino and save LUT
'''
import argparse
import csv
import logging

logging.basicConfig(
    format='%(levelname)s %(funcName)s %(lineno)s: %(message)s', 
    level=logging.DEBUG)


def build_lut(profile_path):
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
    lut_data = [None] * 1023
    lut_data[0] = 0

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


    logging.debug(len(lut_data))
    logging.debug(lut_data)




def cli_args():
    parser = argparse.ArgumentParser(description='__doc__')

    parser.add_argument(
        'profile',
        help='Profile file, should have a column of milliseconds, ' \
            + 'and a tab separated column of analogRead() values')

    return parser.parse_args()


if __name__ == '__main__':
    args = cli_args()
    build_lut(args.profile)
