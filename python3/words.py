#!/usr/bin/env python3

import string
import sys

def make_dictionary():
    with open('../dictionaries/en-US.dic') as file:
        return { line.rstrip().split('/')[0] for line in file }

def cost_from_word_to_goal(word, goal):
    cost = 0
    for index, letter, in enumerate(word):
        if letter != goal[index]:
            cost = cost + 1

    return cost

def int_find_neighbors(word, goal, possibilities, cost_so_far, my_dictionary):
    """
    Find possible neighbors of 'start' by rotating the letters when the same indexed
    letter in 'goal' does not match.

    :return: a list of possibilities
    """
    for index, letter in enumerate(word):
        if letter != goal[index]:
            for alternate in string.ascii_lowercase:
                possible = word[:index] + alternate + word[index+1:]
                if possible != word and possible in my_dictionary:
                    new_cost = cost_so_far[word] + cost_from_word_to_goal(possible, goal)
                    if possible not in cost_so_far or new_cost < cost_so_far[possible]:
                        possibilities[possible] = word
                        cost_so_far[possible] = new_cost
                        possibilities = int_find_neighbors(possible, goal, possibilities, cost_so_far, my_dictionary)

    return possibilities

def find_neighbors(start, goal, my_dictionary):
    return int_find_neighbors(start, goal, {start:None}, {start:0}, my_dictionary)

def run_program(start, goal):
    my_dictionary = make_dictionary()
    possible_words = find_neighbors(start, goal, my_dictionary)
    
    if len(possible_words) == 1:
        print('no possible solution: %s' % possible_words)
        return []

    solution = []
    next = goal
    while next != start:
        solution.append(next)
        next = possible_words[next]

    solution.append(start)
    solution.reverse()
    return solution

if __name__ == '__main__':
    if len(sys.argv) >= 3:
        start, goal = sys.argv[1:3]

    if len(start) == len(goal):
        solution = run_program(start, goal)
        print('solution: %s' % solution)
    else:
        print('start and goal must have the same number of characters')