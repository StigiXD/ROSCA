#include <eosio/eosio.hpp>
#include "groups.cpp"

using namespace eosio;

class [[eosio::contract("creategroup")]] creategroup : public eosio::contract {
  public:

    creategroup(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}

    [[eosio::action]]
    void create(name user, uint64_t contrib_amt, uint64_t max_usrs) {
        require_auth(user);
        create_group(user, 0, contrib_amt, max_usrs);
    }

  private:

  void create_group(name user, uint64_t group_ID, uint64_t contribution_amt, uint64_t max_usrs){
    groups::create_action create_g("groups"_n, {get_self(), "active"_n});
    persons::join_action join_p("persons"_n, {get_self(), "active"_n});
        
    std::vector<uint64_t> v;
    v.push_back(group_ID);

    create_g.send(user, contribution_amt, 0, max_usrs, 1, 0);

    // need to change this as it creates a new user and does not check if there is already existing one
    join_p.send(user, v, 0, 0);

  }
};