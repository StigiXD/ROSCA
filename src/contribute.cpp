#include <eosio/eosio.hpp>
#include "groups.cpp"

using namespace eosio;

class [[eosio::contract("contribute")]] contribute : public eosio::contract {
  public:

    contribute(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}

    [[eosio::action]]
    void pay(name user, uint64_t groupID) {
        //check if max ammount of users are in group - if not spit out error
        //allow user to contribute to group if max users are reached and update person table accordingly
        require_auth(user);
        pay_contribution(user, groupID);
        
    }

  private:

    void pay_contribution(name user, uint64_t group_ID){
        groups::contribute_action contr("groups"_n, {get_self(), "active"_n});
        contr.send(user, group_ID);
    }
};