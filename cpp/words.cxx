#include <cassert>
#include <fstream>
#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

typedef std::unordered_set<std::string> dictionary_type ;

struct PlanNode {
   std::string word;
   int weight;
   bool is_dead_end;

   PlanNode() : word(), weight(0), is_dead_end(false) {}
};

// calculate the distance from word to the goal.
// Larger values represent that the word is further from the goal
// \param word The evaluated word
// \param goal where we want to arrive
// \returns the calculated weight
static int
calculate_weight(const std::string& word, const std::string& goal)
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

static bool
is_word_in_plan(const std::string& word, const std::vector<PlanNode>& plan)
{
    bool is_in_plan = false;

    for (auto node: plan) {
        is_in_plan = is_in_plan || (node.word == word);
    }

    return is_in_plan;
}

// try permutations
// given start and end, try substituting start character with end characters first
// returning plan node that iterates letters by one on track towards goal
// this function returns an empty node
static std::tuple<PlanNode, bool>
permute(const std::string& start, const std::string& goal, const dictionary_type& dict,
        const std::vector<PlanNode>& plan)
{
    // how to differentiate done, bad input and no direct next step?
   if (start == goal) {
       return std::make_tuple(PlanNode(), false);
   }

   if (start.length() != goal.length()) {
      return std::make_tuple(PlanNode(), false);
   }

   PlanNode node;
   std::string next;
   for (int i = 0; i < start.length(); ++i) {
      // handle simple cases where using the letter from goal makes a word
        next = start;
        next[i] = goal[i];
        if (next != start && !is_word_in_plan(next, plan) && dict.count(next)) {
            node.word = next;
            node.weight = calculate_weight(next, goal);
            break;
        } else {
            next.clear();
        }
   }

   if (next.empty()) {
       // no easy next step. Try to permute the start word to a real word that moves us closer
       // to the goal.
       for (int i = 0; i < start.length(); ++i) {
            next = start;
            // hack: take advantage of ASCII ordering. Don't worry about unicode at this time
            next[i] = 'A';
            while (next[i] <= 'z') {
                next[i] = next[i] + 1;
                if (next != start && !is_word_in_plan(next, plan) && dict.count(next)) {
                    node.word = next;
                    node.weight = calculate_weight(next, goal);
                    break;
                }
            }
       }
   }

   return std::make_tuple(node, !node.word.empty());
}

static int plan_sum(const std::vector<PlanNode>& plan)
{
    int sum = 0;
    for (auto node: plan) {
        sum += node.weight;
    }
    return sum;
}

static bool operator<(const std::vector<PlanNode>& lhs, const std::vector<PlanNode>& rhs)
{
    return plan_sum(lhs) < plan_sum(rhs);
}

class Plan {
public:
    Plan() : m_plan(), m_success(false) {}

    int cost() const
    {
        return plan_sum(m_plan);
    }

    bool success() const { return m_success; }

    void print() const
    {
        for (const auto& node: m_plan) {
            std::cout << "\t" << (node.is_dead_end ? "[DEAD-END]" : node.word) << " (" << node.weight << ")\n";
        }
    }

    void develop(const std::string& start, const std::string& goal, const dictionary_type& dict)
    {
        std::priority_queue<std::vector<PlanNode>, std::vector<std::vector<PlanNode>>> plans;

        //std::vector<PlanNode> plan;
        std::tie(plans, m_success) = develop_plan(start, goal, dict, m_plan);
        //plans.push(plan);

        // if (!plans.empty()) {
        //     m_plan = plans.top();
        // }
    }
private:
    std::vector<PlanNode> m_plan;
    bool m_success;

    std::tuple<std::vector<PlanNode>, bool>
    develop_plan(const std::string& start, const std::string& goal, const dictionary_type& dict,
                std::vector<PlanNode>& plan)
    {
        auto result = permute(start, goal, dict, plan);
        bool ok = false;
        if (std::get<1>(result)) {
            auto node = std::get<0>(result);
            assert(!node.word.empty());
            auto sum = 0;
            
            if (node.word != goal) {
                auto element = plan.emplace_back(node);
                std::tie(plan, ok) = develop_plan(element.word, goal, dict, plan);
            } else {
                ok = true;
            }
        } else {
            ok = false;
            PlanNode dead_end;
            dead_end.weight = std::numeric_limits<int>::max();
            dead_end.is_dead_end = true;
            plan.emplace_back(dead_end);
        }
        return std::make_tuple(plan, ok);
    }
};
static bool operator<(const Plan& lhs, const Plan& rhs)
{
    return lhs.cost() < rhs.cost();
}

static int
init_dict(dictionary_type& dict, const std::string& filename)
{
    int ec = 0;
    std::ifstream strm(filename, std::ios::in);
    if (strm) {
        std::string word;
        while (std::getline(strm, word)) {
            dict.emplace(word);
            word.clear();
        }
    } else {
        ec = -1;
    }
    return ec;
}

static Plan
find_best_plan(const std::string& start, const std::string& goal, const dictionary_type& dict)
{
    Plan plan;
    plan.develop(start, goal, dict);
    return plan;
}

int
main(int argc, char** argv)
{
    std::string start;
    std::string goal;
    
    if (argc == 3) {
        start = argv[1];
        goal = argv[2];
    } else {
        std::cout << "enter start and end words (same length), separated by a space:\n";
        std::cin >> start >> goal;
    }
    
    if (start.length() != goal.length()) {
        std::cerr << "start and end goals must be the same length.\n";
        return -1;
    }

    dictionary_type dict;
    if (int ec = init_dict(dict, "../dictionaries/en-US.dic")) {
        std::cerr << "failed to init dictionary: " << ec << "\n";
        return -1;
    }

    const Plan plan = find_best_plan(start, goal, dict);

    std::cout << "Plan:\n";
    std::cout << "* " << start << std::endl;
    plan.print();
    if (plan.success()) {
        std::cout << "* " << goal << std::endl;
    } else {
        std::cout << "FAIL" << std::endl;
    }

    return 0;
}