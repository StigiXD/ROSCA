#include <eosio/eosio.hpp>
#include <cstdio>
#include "persons.cpp"

using namespace eosio;

class [[eosio::contract("groups")]] groups : public eosio::contract {
  public:  
    groups(name receiver, name code, datastream<const char*> ds):contract(receiver, code, ds) {}
 
    [[eosio::action]]
    void create(
        name user,
        //uint64_t group_ID,
        uint64_t contribution_amt,
        uint64_t total_contrib_round,
        uint64_t max_usrs,
        uint64_t joined_usrs,
        uint64_t current_round
    ) {
        require_auth(name("creategroup"));
        group_index group(get_self(), get_first_receiver().value);
        //auto iterator = group.find(group_ID);
        //check (iterator == group.end(), "Record already exists!");
        std::vector<std::string> v;

        group.emplace(get_self(), [&]( auto& row ) {
            //row.group_ID = group_ID;
            row.group_ID = group.available_primary_key();
            row.contribution_amt = contribution_amt;
            row.total_contrib_round = total_contrib_round;
            row.max_usrs = max_usrs;
            row.joined_usrs = joined_usrs;
            row.current_round = current_round;
            row.v.push_back(user.to_string());
            });
    }
    
    [[eosio::action]]
    void join(name user, uint64_t group_ID) {
        require_auth(name("joingroup"));
        group_index group(get_self(), get_first_receiver().value);
        auto iterator = group.find(group_ID);
        check (iterator != group.end(), "Group does not exist!");
        uint64_t max = iterator->max_usrs;
        uint64_t joined = iterator->joined_usrs;
        check(joined != max, "Group limit reached!");
        group.modify(iterator, get_self(), [&]( auto& row ) {
            row.joined_usrs = joined+1;
            row.v.push_back(user.to_string());
            });

    }

    [[eosio::action]]
    void erase(uint64_t group_ID){
      require_auth(get_self());
      group_index group(get_self(), get_first_receiver().value);
      auto iterator = group.find(group_ID);
      check(iterator != group.end(), "Record does not exist");
      group.erase(iterator);
    }

    [[eosio::action]]
    void contribute(name user, uint64_t group_ID){
      //require_auth(name("contribute"));
      check(has_auth(name("contribute")) || has_auth(name("reward")), "No auth");
      group_index group(get_self(), get_first_receiver().value);
      auto iterator = group.find(group_ID);
      check (iterator != group.end(), "Group does not exist!");
      uint64_t t = iterator->total_contrib_round;
      uint64_t amt = iterator->contribution_amt;
      uint64_t max = iterator->max_usrs;
      uint64_t joined = iterator->joined_usrs;
      uint64_t crt_rnd = iterator->current_round;
      std::vector<std::string> vec = iterator->v;
  
      if (joined == max){
        if(crt_rnd+1 != max){
          if(t < max * amt){
            contribute_person(user, group_ID, amt);
            group.modify(iterator, get_self(), [&]( auto& row ) {
                row.total_contrib_round = t+amt;
                });
          }
          else{
            reward(group_ID);
          }
        }
        else{
          //distrbiute prizes if not
          //disband group and delete from groups table and persons table
          //alternatively, keep history of groups - but need to implement incrementation in on create group cpp 
          printf("Max rounds reached!");
          reward(group_ID);
          erase(group_ID);
        }
      }
      else{
        printf("Issue with contributing - check group limit!");
      }
    }


    [[eosio::action]]
    void reward(uint64_t group_ID){
      require_auth(name("reward"));
      group_index group(get_self(), get_first_receiver().value);
      auto iterator = group.find(group_ID);
      check (iterator != group.end(), "Group does not exist!");
      uint64_t t = iterator->total_contrib_round;
      uint64_t amt = iterator->contribution_amt;
      uint64_t max = iterator->max_usrs;
      uint64_t joined = iterator->joined_usrs;
      uint64_t crt_rnd = iterator->current_round;
      std::vector<std::string> vec = iterator->v;
      auto it1 = vec.begin();
      auto it2 = vec.end();
      auto temp = vec.front();
      vec.erase(it1, it1+1);
      print(name(temp));

      reward_person(name(temp), group_ID, (amt*max));
      group.modify(iterator, get_self(), [&]( auto& row ) {
          row.total_contrib_round = 0;
          row.v = vec;
          row.current_round = crt_rnd+1;
          });

      printf("Hello5");
    }
    using create_action = action_wrapper<"create"_n, &groups::create>;
    using join_action = action_wrapper<"join"_n, &groups::join>;
    using contribute_action = action_wrapper<"contribute"_n, &groups::contribute>;

  private:
    struct[[eosio::table]] group {
        uint64_t group_ID;
        uint64_t contribution_amt;
        uint64_t total_contrib_round;
        uint64_t max_usrs;
        uint64_t joined_usrs;
        std::vector<std::string> v;
        uint64_t current_round;


        uint64_t primary_key() const { return group_ID;}
    };

    void contribute_person(name user, uint64_t group_ID, uint64_t amt){
      persons::contribute_action contr_p("persons"_n, {get_self(), "active"_n});

      contr_p.send(user, group_ID, amt);
    }

    void reward_person(name user, uint64_t group_ID, uint64_t amt){
      persons::reward_action reward_p("persons"_n, {get_self(), "active"_n});

      reward_p.send(user, group_ID, amt);
    }
    
   // int roll_die(uint64_t usrs) {
   //   boost::random::uniform_int_distribution<> dist(1, usrs);
   //   return dist(gen);
   // }
    using group_index = eosio::multi_index<"groups"_n, group>;

};