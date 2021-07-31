# Interview Prep

## Question

Given two five letter words, A and B, and a dictionary of five letter words, find a shortest transformation from A to B, such that only one letter can be changed at a time and all intermediate words in the transformation must exist in the dictionary.
For example, if A and B are “smart” and “brain”, the result may be:

```
smart
   start
   stark
   stack
   slack
   black
   blank
   bland
   brand
   braid
brain
```

Please also include test cases against your algorithm.
My hope is that this will be roughly two hours or so worth of work. You can choose to implement your solution in any language you like. I’d like you to submit your solution on GitHub.
As an additional option, once you have a complete solution to the base problem, please provide a variant implementation takes advantage of multiple CPU cores.

## Solution

So, I need to come up with a plan to transition to the end answer. I probably need some sort of weighting system evalulate "moves," similar to A* path planning algorithm.

- I could find all of the solutions, measuring the distance to the final product as I go
- then choose to pursue the most promising solutions paths.

*Decisions*
- How do I check if a word is in the dictionary? I could start with a fixed length spelling dictionary in a standalone file.
   [hunspell](https://github.com/hunspell/hunspell) appears to be the standard format for this type of thing.
   standard hunspell [dictionaries](https://github.com/wooorm/dictionaries)
   the [format](https://www.systutorials.com/docs/linux/man/4-hunspell/) is quite simple.
   There are more features, but I can start this project with a very simple "personal" file with one word per line. There is
   not any need to support affix and other "flags" at this time.
- What is my weighting algorithm?


## C++ Pseuduo Code

```
unordered_map<string> my_dictionary;
for line in en-US.dic:
  my_dictionary.emplace(line)

// enter start (A) and end (B) words 
string start;
string end;
cout << "enter start and end words, separated by a space:\n";
cin >> start >> end;

if (start.length() != end.length()) {
   cout << "words must be same length.\n";
   return -1;
}

struct PlanNode {
   string word;
   int weight;
};
// need a way to try different solutions. Do I need to store intermediate steps?
vector<PlanNode> my_plan;

// calculate the distance from word to the goal.
// Larger values represent that the word is further from the goal
// \param word The evaluated word
// \param goal where we want to arrive
// \returns the calculated weight
int calculate_weight(string word, string goal)
{
   if (word.length() != goal.length()) {
      return 10000;
   }

   // simple, but good enough???
   int weight = 0;
   for (int i = 0; i < word.length(); ++i) {
      if (word[i] != goal[i]) {
         weight += 1;
      }
   }
   return weight;
}

// try permutations
// given start and end, try substituting start character with end characters first
PlanNode permute(string start, string goal, unordered_map<string> dict) {
   if (start == goal) {
      return;
   }
   if (start.length() != goal.length()) {
      return;
   }

   string next;
   for (int i = 0; i < start.length(); ++i) {
      // handle simple cases where using the letter from goal makes a word
      if (start[i] != goal[i]) {
         next = start;
         next[i] = goal[i];
         if (next in dict) {
            // compute weight?
         } else {
            next.clear();
         }
      }
   }
}
```
