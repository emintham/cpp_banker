import random
import sys

from collections import namedtuple
from itertools import islice

from pexpect import spawn


# Tile types
REGULAR = 0
NONPROFIT = 1
NEGATIVELAWSUIT = 2
POSITIVELAWSUIT = 3
COMPETITOR = 4

GameInfo = namedtuple('GameInfo', ['board', 'score', 'cash', 'moveset', 'next_tile'])
Tile = namedtuple('Tile', ['value', 'type'])


class GameEnd(Exception):
    """Exception raised when game has ended"""
    pass


class UnknownTile(Exception):
    """Exception raised when a given tile is unknown"""
    pass


def grouper(n, iterable):
    """Yields n objects from an iterable at a time as a list"""
    it = iter(iterable)

    while True:
        chunk = list(islice(it, n))
        if not chunk:
            return

        yield chunk


def decode(b_obj):
    """Decodes bytes object"""
    return b_obj.decode('utf-8')


def decode_line(line):
    """Decodes a line and splits it by whitespace"""
    return decode(line).split()


def decode_move(move_str):
    """Decodes a string representing a move"""
    move = map(int, move_str.split(','))
    return tuple(move)


def encode_move(move):
    """
    Takes a tuple representing a source and destination and encodes it in a
    format recognizable by banker.
    """
    return '{} {}'.format(*move)


def decode_tile(tile_str):
    """Decodes a string representing a tile"""
    tile_str = decode(tile_str)

    try:
        value = int(tile_str)
    except ValueError:
        if tile_str == '(0)':
            return Tile(0, COMPETITOR)
        elif tile_str == '+':
            return Tile(0, POSITIVELAWSUIT)
        elif tile_str == '-':
            return Tile(0, NEGATIVELAWSUIT)
        elif tile_str.endswith('*'):
            return Tile(int(tile_str[:-1]), NONPROFIT)

    if value > 0:
        return Tile(value, REGULAR)
    elif value == 0:
        raise GameEnd
    elif value < 0:
        return Tile(-value, COMPETITOR)

    raise UnknownTile


def read_game_info(proc):
    """
    Takes an open child process running banker and returns a tuple containing
    the information about the game.
    """
    raw_board = decode_line(proc.readline().strip())
    board = [chunk for chunk in grouper(5, raw_board)]

    score, cash = map(int, decode_line(proc.readline().strip()))

    raw_moveset = decode_line(proc.readline().strip())
    moveset = [decode_move(move) for move in raw_moveset]

    tile = decode_tile(proc.readline().strip())

    return GameInfo(board, score, cash, moveset, tile)


def str_board(board):
    return '\n'.join(str(row) for row in board)


def get_banker_session():
    try:
        return spawn('/Users/etham/cpp/banker/banker')
    except ExceptionPexpect:
        raise Exception('`banker` not found, did you run `make banker`?')


def random_player(game_info):
    """Takes the game information and returns a random but valid move"""
    return random.choice(game_info.moveset)


class Banker:
    """
    Banker class to interact with the game.
    """

    def __init__(self, play_policy, verbose=False):
        self.session = get_banker_session()
        self.play_policy = play_policy
        self.verbose = verbose

    def run(self):
        while(True):
            try:
                game_info = read_game_info(self.session)
            except (GameEnd, UnknownTile):
                break

            print(str_board(game_info.board))
            print('Score: {}, Cash: {}\n'.format(game_info.score, game_info.cash))

            move = self.play_policy(game_info)
            encoded_move = encode_move(move)
            bytes_written = self.session.sendline(encoded_move)

            if (self.verbose):
                print('Going from {} to {}'.format(*random_move))
                print('Sent {} bytes\n'.format(bytes_written))

            # read the sent line
            self.session.readline()

        self.session.close()


if __name__ == '__main__':
    verbose = sys.argv[1] == '-v' if len(sys.argv) > 1 else False
    banker = Banker(random_player, verbose)

    banker.run()
