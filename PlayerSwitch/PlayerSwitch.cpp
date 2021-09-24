#include "pch.h"
#include "PlayerSwitch.h"

#include <random>

BAKKESMOD_PLUGIN(PlayerSwitch, "Switches the position from 2 players every x seconds", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void PlayerSwitch::onLoad() {
	_globalCvarManager = cvarManager;

	//Register CVars
	cvarManager->registerCvar("playerswitch_enabled", "0", "Enable PlayerSwitch", true, true, 0, true, 1);
	cvarManager->registerCvar("playerswitch_static_enabled", "0", "Enable for static time", true, true, 0, true, 1);
	cvarManager->registerCvar("playerswitch_dynamic_enabled", "0", "Enable for dynamic time", true, true, 0, true, 1);

	cvarManager->registerCvar("playerswitch_switch_boost", "1", "Enable boost switching", true, true, 0, true, 1);
	cvarManager->registerCvar("playerswitch_switch_velocity", "1", "Enable velocity switching", true, true, 0, true, 1);
	cvarManager->registerCvar("playerswitch_switch_rotation", "1", "Enable rotation switching", true, true, 0, true, 1);

	cvarManager->registerCvar("playerswitch_time_difference_static", "30.0", "Time in seconds", true, true, TIME_DIFFERENCE_MIN, true, TIME_DIFFERENCE_MAX, 1);
	cvarManager->registerCvar("playerswitch_time_difference_dynamic_min", "20.0", "Min-Time in seconds", true, true, TIME_DIFFERENCE_MIN, true, TIME_DIFFERENCE_MAX, 1);
	cvarManager->registerCvar("playerswitch_time_difference_dynamic_max", "40.0", "Max-Time in seconds", true, true, TIME_DIFFERENCE_MIN, true, TIME_DIFFERENCE_MAX, 1);

	//Register notifiers
	cvarManager->registerNotifier("switch_players", [this](std::vector<std::string> args) {
		switchPlayers();
	}, "", PERMISSION_ALL);

	//Event-Hooks
	gameWrapper->HookEvent("Function TAGame.GameEvent_TA.EventMatchStarted", bind(&PlayerSwitch::GameStart, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function Engine.DateTime.EpochNow", bind(&PlayerSwitch::EpochNow, this, std::placeholders::_1));
}

void PlayerSwitch::onUnload() {}

/*
 * This function is called when the bein countdown starts
 * Performs setup
 */
void PlayerSwitch::GameStart(std::string eventName) {
	last_switch = 0.0;
	next_random_dynamic_switch = (float)createNextRandomSwitch();
}

/*
 * This function is called every second
 * Checks if players can be switched and if so performs it
 */
void PlayerSwitch::EpochNow(std::string eventName) {
	CVarWrapper enableCvar = cvarManager->getCvar("playerswitch_enabled");
	if (!enableCvar) { return; }
	if (!enableCvar.getBoolValue()) { return; }

	CVarWrapper enableStaticCvar = cvarManager->getCvar("playerswitch_static_enabled");
	CVarWrapper enableDynamicCvar = cvarManager->getCvar("playerswitch_dynamic_enabled");

	if (enableStaticCvar.getBoolValue()) {
		performSwitchStatic();
	} else if (enableDynamicCvar.getBoolValue()) {
		performSwitchDynamic();
	}
}

/*
 * Validates the time for static switching and performs the switchign afterwards (if enabled)
 */
void PlayerSwitch::performSwitchStatic() {
	ServerWrapper sw = gameWrapper->GetGameEventAsServer();
	if (!sw) { return; }

	float time_played = sw.GetTotalGameTimePlayed();
	float switch_difference = time_played - last_switch;

	float static_time_difference = getStaticSwitchCvarValue();
	if (switch_difference >= static_time_difference) {
		cvarManager->executeCommand("switch_players");
		last_switch = time_played;
	}
}

/*
 * Validates the time for dynamic switching and performs the switchign afterwards (if enabled)
 */
void PlayerSwitch::performSwitchDynamic() {
	ServerWrapper sw = gameWrapper->GetGameEventAsServer();
	if (!sw) { return; }

	float time_played = sw.GetTotalGameTimePlayed();
	float switch_difference = time_played - last_switch;
	if (switch_difference >= next_random_dynamic_switch) {
		cvarManager->executeCommand("switch_players");
		last_switch = time_played;

		next_random_dynamic_switch = createNextRandomSwitch();
	}
}

/*
 * Performs the switching
 */
void PlayerSwitch::switchPlayers() {
	PriWrapper player1 = getPlayer1();
	if (!player1) { return; }
	
	std::string id_string_p1 = player1.GetUniqueIdWrapper().GetIdString();
	std::string name_p1 = player1.GetPlayerName().ToString();

	PriWrapper player2 = getPlayer2(id_string_p1, name_p1);
	if (!player2) { return; }

	CarWrapper car_p1 = player1.GetCar();
	if (!car_p1) { return; }
	CarWrapper car_p2 = player2.GetCar();
	if (!car_p2) { return; }

	switchLocation(car_p1, car_p2);
	switchVelocity(car_p1, car_p2);
	switchRotation(car_p1, car_p2);
	switchBoost(car_p1, car_p2);
}

/*
 * Switches the location between two players
 */
void PlayerSwitch::switchLocation(CarWrapper car_p1, CarWrapper car_p2) {
	Vector location_p1 = car_p1.GetLocation();
	Vector location_p2 = car_p2.GetLocation();

	car_p1.SetLocation(location_p2);
	car_p2.SetLocation(location_p1);
}

/*
 * Switches the boost between two players
 */
void PlayerSwitch::switchBoost(CarWrapper car_p1, CarWrapper car_p2) {
	CVarWrapper switchBoostCvar = cvarManager->getCvar("playerswitch_switch_boost");
	if (!switchBoostCvar) { return; }
	if (!switchBoostCvar.getBoolValue()) { return; }

	BoostWrapper bw_p1 = car_p1.GetBoostComponent();
	if (!bw_p1) { return; }
	BoostWrapper bw_p2 = car_p2.GetBoostComponent();
	if (!bw_p2) { return; }

	float boost_p1 = car_p1.GetBoostComponent().GetCurrentBoostAmount();
	float boost_p2 = car_p2.GetBoostComponent().GetCurrentBoostAmount();

	bw_p1.SetBoostAmount(boost_p2);
	bw_p2.SetBoostAmount(boost_p1);
}

/*
 * Switches the velocity between two players
 */
void PlayerSwitch::switchVelocity(CarWrapper car_p1, CarWrapper car_p2) {
	CVarWrapper switchVelocityCvar = cvarManager->getCvar("playerswitch_switch_velocity");
	if (!switchVelocityCvar) { return; }
	if (!switchVelocityCvar.getBoolValue()) { return; }

	Vector velocity_p1 = car_p1.GetVelocity();
	Vector velocity_p2 = car_p2.GetVelocity();

	car_p1.SetVelocity(velocity_p2);
	car_p2.SetVelocity(velocity_p1);
}

/*
 * Switches the rotation between two players
 */
void PlayerSwitch::switchRotation(CarWrapper car_p1, CarWrapper car_p2) {
	CVarWrapper switchrotationCvar = cvarManager->getCvar("playerswitch_switch_rotation");
	if (!switchrotationCvar) { return; }
	if (!switchrotationCvar.getBoolValue()) { return; }

	Rotator rotation_p1 = car_p1.GetRotation();
	Rotator rotation_p2 = car_p2.GetRotation();

	car_p1.SetRotation(rotation_p2);
	car_p2.SetRotation(rotation_p1);
}

/*
 * Gets player1 to switch with various conditions
 */
PriWrapper PlayerSwitch::getPlayer1() {
	ServerWrapper sw = gameWrapper->GetGameEventAsServer();
	if (!sw) { return NULL; }

	ArrayWrapper<PriWrapper> pris = sw.GetPRIs();
	int count_players_ingame = 0;
	for (int i = 0; i < pris.Count(); i++) { //counts players ingame to later check if there are enough players ingame (and not spectating)
		PriWrapper pri = pris.Get(i);

		int team_num_2 = pri.GetTeamNum2();
		if (team_num_2 == TEAM_NUM_BLUE || team_num_2 == TEAM_NUM_ORANGE) {
			count_players_ingame++; //this logic is only checking for players ingame and not specific in which team they are
		}
	}

	if (count_players_ingame < 2) {
		return NULL;
	}

	int run = 0;
	while (run < 250) {
		int random_int = createRandomInt(pris.Count());
		PriWrapper pri = pris.Get(random_int);

		int team_num_2 = pri.GetTeamNum2();
		if (team_num_2 == TEAM_NUM_BLUE || team_num_2 == TEAM_NUM_ORANGE) {
			cvarManager->log("getPlayer1(): added player:" + pri.GetPlayerName().ToString() + " (team_num_2: " + std::to_string(team_num_2) + ")");
			return pri;
		}
		cvarManager->log("getPlayer1(): tried to add player: " + pri.GetPlayerName().ToString() + " (team_num_2: " + std::to_string(team_num_2) + ")");
		run++;
	}

	return NULL; //returns NULL if random generation could not find any player after 250 tries (used to prevent infinite loop)
}

/*
 * Gets player2 to switch with various conditions 
 */
PriWrapper PlayerSwitch::getPlayer2(std::string id_string_p1, std::string name_p1) {
	ServerWrapper sw = gameWrapper->GetGameEventAsServer();
	if (!sw) { return NULL; }

	ArrayWrapper<PriWrapper> pris = sw.GetPRIs();
	int run = 0;
	while (run < 250) {
		int random_int = createRandomInt(pris.Count());
		PriWrapper pri = pris.Get(random_int);

		int team_num_2 = pri.GetTeamNum2();
		if (team_num_2 == TEAM_NUM_BLUE || team_num_2 == TEAM_NUM_ORANGE) {
			std::string id_string_p2 = pri.GetUniqueIdWrapper().GetIdString();
			std::string name_p2 = pri.GetPlayerName().ToString();
			cvarManager->log("Player: " + pri.GetPlayerName().ToString() + " | " + id_string_p2);
			if (!isSamePlayer(id_string_p1, id_string_p2, name_p1, name_p2)) {
				cvarManager->log("getPlayer2(): added player:" + pri.GetPlayerName().ToString() + " (team_num_2: " + std::to_string(team_num_2) + ")");
				return pri;
			}
		}
		cvarManager->log("getPlayer2(): tried to add player: " + pri.GetPlayerName().ToString() + " (team_num_2: " + std::to_string(team_num_2) + ")");
		run++;
	}

	return NULL;
}

/**
 * Creates a random integer in the range of 0 to <player_count - 1>
 */
int PlayerSwitch::createRandomInt(int player_count) {
	std::random_device rd; // only used once to initialise (seed) engine
	std::mt19937 rng(rd()); // random-number engine used (Mersenne-Twister in this case)
	std::uniform_int_distribution<int> uni(0, (player_count - 1)); // guaranteed unbiased
	auto random_integer = uni(rng);

	return random_integer;
}

/*
 * Generates the new value for the next dynamic switch
 */
float PlayerSwitch::createNextRandomSwitch() {
	CVarWrapper dynamicMinCvar = cvarManager->getCvar("playerswitch_time_difference_dynamic_min");
	if (!dynamicMinCvar) { return 1.337f; }
	CVarWrapper dynamicMaxCvar = cvarManager->getCvar("playerswitch_time_difference_dynamic_max");
	if (!dynamicMaxCvar) { return 1.337f; }

	int min = dynamicMinCvar.getIntValue();
	int max = dynamicMaxCvar.getIntValue();

	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uni(min, max);
	auto random_integer = uni(rng);

	return random_integer;
}

/*
 * Returns the value of the staticTimeDifferenceCvar
 */
int PlayerSwitch::getStaticSwitchCvarValue() {
	CVarWrapper staticTimeDifferenceCvar = cvarManager->getCvar("playerswitch_time_difference_static");
	if (!staticTimeDifferenceCvar) { return 20; }

	return staticTimeDifferenceCvar.getIntValue();
}

/*
 * Checks if the two players are the same
 */
bool PlayerSwitch::isSamePlayer(std::string id_string_p1, std::string id_string_p2, std::string name_p1, std::string name_p2) {
	if (id_string_p1 != id_string_p2) {
		return false;
	} else if (id_string_p1 == id_string_p2 && id_string_p1 == "Unknown|0|0") { //Checks for bots
		if (name_p1 == name_p2) {
			return true;
		} else {
			return false;
		}
	}

	return true; // for crash purposes (should not happen since there are only players (1st) and bots (2nd))
}
