#include "pch.h"
#include <EventAPI.h>
#include <LoggerAPI.h>
#include <MC/Level.hpp>
#include <MC/BlockInstance.hpp>
#include <MC/Block.hpp>
#include <MC/BlockSource.hpp>
#include <MC/Actor.hpp>
#include <MC/Player.hpp>
#include <MC/ItemStack.hpp>
#include <LLAPI.h>
#include <MC/Player.hpp>
#include <MC/BlockActor.hpp>
#include <random>
Logger Tc("TreasureChest");
int a = 1;
//{"Findable":0b,"Items":[],"id":"Chest","isMovable":1b,"x":-328,"y":69,"z":-333}
//{"Findable":1b,"Items":[],"LootTable":"loot_tables/chests/pillager_outpost.json","LootTableSeed":879397998,"id":"Chest","isMovable":1b,"x":-2218,"y":77,"z":425}
void entry()
{
	LL::registerPlugin("TreasureChest", "TreasureChest", LL::Version(1, 0, 0));
	Event::PlayerChatEvent::subscribe([](Event::PlayerChatEvent e)-> bool {//use to test
		BlockPos pos = e.mPlayer->getPosition();
		Level::setBlock(pos, 0, "minecraft:chest", 54);
		BlockSource *bs = Level::getBlockSource(0);
		BlockActor *blac = Level::getBlockEntity(pos, bs);
		auto nbt = blac->getNbt();
        {
			std::default_random_engine e(time(0));
			nbt->putString("LootTable", "loot_tables/chests/pillager_outpost.json");
			nbt->putInt("LootTableSeed",e());
        }
		blac->setNbt(nbt->asCompoundTag());
		return true;
		});
}

THook(void, "?tick@ServerLevel@@UEAAXXZ",
	void* _this)
{
	a++;
	Tc.info("{}",a);
	if (Level::getAllPlayers().size() != 0 && a%200 == 0)
	{
		for (Player *pl:Level::getAllPlayers())
		{
			BlockPos pos = pl->getPosition();
			Level::setBlock(pos, 0, "minecraft:chest", 54);
			BlockSource* bs = Level::getBlockSource(0);
			BlockActor* blac = Level::getBlockEntity(pos, bs);
			auto nbt = blac->getNbt();
			{
				std::default_random_engine e(time(0));
				nbt->putString("LootTable", "loot_tables/chests/pillager_outpost.json");
				nbt->putInt("LootTableSeed", e());
			}
			blac->setNbt(nbt->asCompoundTag());
		}
	}
	return original(_this);
}