import random
import argparse

parser = argparse.ArgumentParser(description='Generate N random colors in RGB format.')
parser.add_argument('N', metavar='N', type=int,
                    help='# random colors to generate')

args = parser.parse_args()

for k in range(0, args.N):
    red =  random.randint(0, 255)
    green =  random.randint(0, 255)
    blue =  random.randint(0, 255)
    print(f'{red}, {green}, {blue}')
