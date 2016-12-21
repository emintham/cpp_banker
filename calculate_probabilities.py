from collections import defaultdict


def read_from_file(filename='tiles.txt'):
    tiles = defaultdict(lambda: defaultdict(int))

    with open(filename, 'rb') as f:
        for line in f:
            tile, score = line.split(' ')
            if tile.startswith('$'):
                continue

            score = int(score)
            score_category = score / 100
            tiles[score_category][tile] += 1

    return tiles


def print_probabilities(d):
    for score_category, dictionary in d.iteritems():
        total = float(sum(dictionary.values()))
        print 'score_category: {}'.format(score_category)
        for tile, number in dictionary.iteritems():
            print '{}: {:.3f}'.format(tile, number/total)


if __name__ == '__main__':
    d = read_from_file()
    print_probabilities(d)
