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
#include "json.hpp"
using json = nlohmann::json;
Logger Tc("TreasureChest");
int a = 0;
#include <MC/Dimension.hpp>
json config;
map<string, int>times;
std::default_random_engine e(time(0));
json readConfigJson() {
	std::ifstream i("plugins/TreasureChest/config.json");
	json j;
	i >> j;
	return j;
}

//{"Findable":0b,"Items":[],"id":"Chest","isMovable":1b,"x":-328,"y":69,"z":-333}
//{"Findable":1b,"Items":[],"LootTable":"loot_tables/chests/pillager_outpost.json","LootTableSeed":879397998,"id":"Chest","isMovable":1b,"x":-2218,"y":77,"z":425}
void entry()
{
	LL::registerPlugin("TreasureChest", "TreasureChest", LL::Version(1, 0, 0));
	config = readConfigJson();
	int x, y, z, dim, time1;
	string path;
	for (json::iterator element = config.begin(); element != config.end(); ++element) {
		Tc.info << element.key() << " : " << element.value() << Tc.endl;
			try
			{
				x = element.value()["x"];
				y = element.value()["y"];
				z = element.value()["z"];
				dim = element.value()["dim"];
				path = element.value()["path"];
				time1 = element.value()["time"];
			}
			catch (std::exception&)
			{
				Tc.error("存在不完整的配置项目: " + element.key() + "!");
				times[element.key()] = 0;
				continue;
			}
			times[element.key()] = time1;
			//Tc.info << times[element.key()] << Tc.endl;
	}
	//Event::PlayerChatEvent::subscribe([](Event::PlayerChatEvent e)-> bool {//use to test
	//	BlockPos pos = e.mPlayer->getPosition();
	//	Level::setBlock(pos, 0, "minecraft:chest", 54);
	//	BlockSource *bs = Level::getBlockSource(0);
	//	BlockActor *blac = Level::getBlockEntity(pos, bs);
	//	auto nbt = blac->getNbt();
 //       {
	//		std::default_random_engine e(time(0));
	//		nbt->putString("LootTable", "loot_tables/chests/pillager_outpost.json");
	//		nbt->putInt("LootTableSeed",e());
 //       }
	//	blac->setNbt(nbt->asCompoundTag());
	//	return true;
	//	});
}

bool SpawnChest(BlockPos pos,string path,int dim) {
	if (Level::getBlockEx(pos,dim) == nullptr)
	{
		Tc.warn(pos.toString() + " " + "此处未加载！");
		return false;
	}
	Level::setBlock(pos, dim, "minecraft:chest", 1);
	BlockSource* bs = Level::getBlockSource(dim);
	BlockActor* blac = Level::getBlockEntity(pos, bs);
	auto nbt = blac->getNbt();
	{
		nbt->putString("LootTable", path);
		nbt->putInt("LootTableSeed", e());
	}
	blac->setNbt(nbt->asCompoundTag());
	return true;
}

THook(void, "?tick@ServerLevel@@UEAAXXZ",
	void* _this)
{
	a+=1;
	int x, y, z, dim, sort = 0;
	string path;
	BlockPos pos;
	json::iterator element;
	for (element = config.begin(); element != config.end(); ++element) {
		//Tc.info << element.key() << Tc.endl;
		if (times[element.key()] != 0 && a % (times[element.key()] * 60 * 10) == 0)
		{
			x = element.value()["x"];
			y = element.value()["y"];
			z = element.value()["z"];
			dim = element.value()["dim"];
			path = element.value()["path"];
			pos = BlockPos(x, y, z);
			SpawnChest(pos, path, dim);
		}
		if (a >= 200000)
			a = 0;
		//return original(_this);
	}
	return original(_this);
}
