#pragma region pots

#define PotReadScale 1000
#define DeadZoneHalfAmplitude 16

int potpin8 = 8;
int potpin9 = 9;
int potpin10 = 10;

int potpin11 = 11;
int potpin12 = 12;
int potpin13 = 13;

int pval8_LS_rot = 0;
int pval9_LS_dU = 0;
int pval10_LS_lR = 0;

int pval11_RS_uD = 0;
int pval12_RS_lR = 0;
int pval13_RS_rot = 0;

struct JOY_ds {
	int16_t LS_lR; //pin10
	int16_t LS_dU; //pin9
	int16_t RS_lR; //pin12
	int16_t RS_uD; //pin11
	int16_t LS_rot;//pin8
	int16_t RS_rot;//pin13
	};
JOY_ds _masterjds;
#pragma endregion

void ReadPotpins() {

// left JS<----------------------
	pval8_LS_rot = analogRead(potpin8);//Rotationccw cw 7 -9-10-11 24
	pval9_LS_dU = analogRead(potpin9); //downup  508+-4 amplitude 170
	pval10_LS_lR = analogRead(potpin10);//rightleft  mid amplitude 170

   //rightJS-------------------------------->
	pval11_RS_uD = analogRead(potpin11);//   512 +-6 amplitude 172
	pval12_RS_lR = analogRead(potpin12); //
	pval13_RS_rot = analogRead(potpin13);//rot  7-->  14-15-16  --->27


	Map01K_update_masterJS();
	}
void Map01K_update_masterJS() {
	//printRawPotValues();
// left JS<----------------------
	pval8_LS_rot = constrain(pval8_LS_rot, 5, 25);
	pval8_LS_rot = deadzonefilter(map(analogRead(potpin8), 5, 25, 0, PotReadScale), true);
	pval9_LS_dU = deadzonefilter(map(analogRead(potpin9), 338, 678, 0, PotReadScale), false);
	pval10_LS_lR = deadzonefilter(map(analogRead(potpin10), 336, 676, 0, PotReadScale), false);
   //rightJS-------------------------------->
	pval11_RS_uD = deadzonefilter(map(analogRead(potpin11), 330, 688,  PotReadScale,0), false);
	pval12_RS_lR = deadzonefilter(map(analogRead(potpin12), 330, 680, 0, PotReadScale), false);
	pval13_RS_rot = constrain(pval13_RS_rot, 0, 25);
	pval13_RS_rot =   deadzonefilter(map(analogRead(potpin13), -1, 25, 0, PotReadScale), true);
//	Serial.println(pval11_RS_uD);

	_masterjds.LS_rot = pval8_LS_rot;
	_masterjds.LS_dU = pval9_LS_dU;
	_masterjds.LS_lR = pval10_LS_lR;

	_masterjds.RS_uD = pval11_RS_uD;
	_masterjds.RS_lR = pval12_RS_lR;
	_masterjds.RS_rot = pval13_RS_rot;

	//printRawPotValues();

	}

//{pval8_LS_rot} {pval9_LS_dU} {pval10_LS_lR} {pval11_RS_uD} {pval12_RS_lR} {pval13_RS_rot}

int deadzonefilter(int argval, bool argisRot) {

	if (argisRot) {
		if ((argval > ((PotReadScale / 2) - (DeadZoneHalfAmplitude +20))) && (argval < ((PotReadScale / 2) + (DeadZoneHalfAmplitude +20))))
			argval = (PotReadScale / 2);

		if (argval < 0)argval = 0;
		if (argval > PotReadScale)argval = PotReadScale;

		}
	else
		{
		if ((argval > ((PotReadScale / 2) - DeadZoneHalfAmplitude)) && (argval < ((PotReadScale / 2) + DeadZoneHalfAmplitude)))
			argval = (PotReadScale / 2);

		if (argval < 0)argval = 0;
		if (argval > PotReadScale)argval = PotReadScale;
		}


	return argval;
	}

void printRawPotValues() {
	String outstr = 
		"LS lr" + String( pval10_LS_lR)+ " ud "+ String(pval9_LS_dU) + "\n" +
		"RS lr" + String(pval12_RS_lR) + " ud " + String(pval11_RS_uD)+"\n" + 
		"L rot " + String(pval8_LS_rot) + " R rot " + String(pval13_RS_rot) + "\n";
	Serial.print(outstr);
	
	}

