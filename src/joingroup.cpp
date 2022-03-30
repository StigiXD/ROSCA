#include <eosio/eosio.hpp>
#include "groups.cpp"

using namespace eosio;

class [[eosio::contract("joingroup")]] joingroup : public eosio::contract {
  public:

    joingroup(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}

    [[eosio::action]]
    void join(name user, uint64_t group_ID) {
        require_auth(user);
        join_group(user, group_ID);
    }

  private:

  void join_group(name user, uint64_t group_ID){
    groups::join_action join_g("groups"_n, {get_self(), "active"_n});
    
    join_g.send(user, group_ID);

    persons::join_action join_p("persons"_n, {get_self(), "active"_n});

    std::vector<uint64_t> v;
    v.push_back(group_ID);

    join_p.send(user, v, 0, 0);

  }
};