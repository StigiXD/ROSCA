#include <eosio/eosio.hpp>
#include "groups.cpp"

using namespace eosio;

class [[eosio::contract("reward")]] reward : public eosio::contract {
  public:

    reward(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}

    [[eosio::action]]
    void winnings(uint64_t group_ID) {
        //check if max ammount of users are in group - if not spit out error
        //allow user to contribute to group if max users are reached and update person table accordingly
        require_auth(get_self());
        rewards(group_ID);
        
    }

  private:

    void rewards(uint64_t group_ID){
        groups::contribute_action contr("groups"_n, {get_self(), "active"_n});
        contr.send(get_self(), group_ID);
    }
};