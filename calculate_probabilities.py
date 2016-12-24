from collections import defaultdict


def read_from_file(filename='tiles.txt'):
    tiles = defaultdict(lambda: defaultdict(int))

    with open(filename, 'rb') as f:
        for line in f:
            tile, score = line.split(' ')
            if tile.startswith('$'):
                continue

            score_category = int(score) / 100
            tiles[score_category][tile] += 1

    return tiles


def print_probabilities(d):
    tile_types = ['2', '1', '0', '-1', '-2', '.2', '.3', '-', '+']

    for score_category, dictionary in d.iteritems():
        total = float(sum(dictionary.values()))
        print 'score < {}:'.format((score_category + 1) * 100)
        for tile_type in tile_types:
            number = dictionary.get(tile_type, 0)
            print '{}: {:.3f}'.format(tile_type, number/total)


if __name__ == '__main__':
    d = read_from_file()
    print_probabilities(d)
