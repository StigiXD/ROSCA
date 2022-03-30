#include <eosio/eosio.hpp>

using namespace eosio;

class [[eosio::contract("persons")]] persons : public eosio::contract {
  public:  
    persons(name receiver, name code, datastream<const char*> ds):contract(receiver, code, ds) {}
 
    [[eosio::action]]
    void join(
        name user,
        std::vector<uint64_t> groups,
        uint64_t contributed_total,
        uint64_t received_total
    ) {
        //require_auth(name("creategroup") || name("groups"));
        check(has_auth(name("creategroup")) || has_auth(name("joingroup")), "No auth");
        person_index person(get_self(), get_first_receiver().value);
        auto iterator = person.find(user.value);


        if(iterator == person.end()){
          person.emplace(get_self(), [&]( auto& row ) {
            row.key = user;
            row.groups = groups;
            row.contributed_total = contributed_total;
            row.received_total = received_total;
            });
        }
        else{
          std::vector<uint64_t> gs = iterator->groups;
          gs.insert(gs.end(), groups.begin(), groups.end());
          person.modify(iterator, get_self(), [&]( auto& row ) {
            row.groups = gs;
            });
        }
    }
    
    [[eosio::action]]
    void contribute(name user, uint64_t group_ID, uint64_t amt){
      require_auth(name("groups"));
      person_index person(get_self(), get_first_receiver().value);
      auto iterator = person.find(user.value);
      check(iterator != person.end(), "Record does not exist");
      uint64_t t = iterator->contributed_total;
      std::vector<uint64_t> v = iterator->groups;
      bool y = false;

      for(auto& it : v) {
        if (it == group_ID){
          printf("Person is member of the group");
          y = true;
          break;
        }
      }

      check(y, "Person is not member of the group!");
      person.modify(iterator, get_self(), [&]( auto& row ) {
            row.contributed_total = t+amt;
            });
    }

    [[eosio::action]]
    void reward(name user, uint64_t group_ID, uint64_t amt){
      printf("Hello2");
      require_auth(name("groups"));
      person_index person(get_self(), get_first_receiver().value);
      auto iterator = person.find(user.value);
      check(iterator != person.end(), "Record does not exist");
      uint64_t t = iterator->received_total;
      std::vector<uint64_t> v = iterator->groups;
      bool y = false;
      printf("Hello3");

      for(auto& x : v){
        if (x == group_ID){
          printf("Person is member of the group");
          y = true;
          break;
        }
      }
      check(y, "Person is not member of the group!");
      person.modify(iterator, get_self(), [&]( auto& row ) {
            row.received_total = t+amt;
            });
    }



    [[eosio::action]]
    void erase(name user){
      require_auth(user);
      person_index persons(get_self(), get_first_receiver().value);
      auto iterator = persons.find(user.value);
      check(iterator != persons.end(), "Record does not exist");
      persons.erase(iterator);
    }
   
    using join_action = action_wrapper<"join"_n, &persons::join>;
    using contribute_action = action_wrapper<"contribute"_n, &persons::contribute>;
    using reward_action = action_wrapper<"reward"_n, &persons::reward>;

  private:
    struct[[eosio::table]] person{
        name key;
        std::vector<uint64_t> groups;
        uint64_t contributed_total;
        uint64_t received_total;

        uint64_t primary_key() const { return key.value;}
    };

    using person_index = eosio::multi_index<"people"_n, person>;

};