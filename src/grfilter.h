#ifndef GRFILTER_H_
#define GRFILTER_H_

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CONFIG_DEBUG 0
#define MAX_GRAPH_SIZE 128
#define MAX_EDGE_NUMBER MAX_GRAPH_SIZE *MAX_GRAPH_SIZE
#define BATCH_SIZE 100

#if CONFIG_DEBUG
void show_coefficients(const double *input, const double *output, int n);
#endif

static const double h4x4_tik[16] = {
    1.0000000000, 0.8722604191, 0.6666666667, 0.5395042868,
    0.8722604191, 0.7734590803, 0.6073686169, 0.5000000000,
    0.6666666667, 0.6073686169, 0.5000000000, 0.4248894476,
    0.5395042868, 0.5000000000, 0.4248894476, 0.3693980625};

static const double h4x4_diff[16] = {
    1.0000000000, 0.7461018061, 0.3678794412, 0.1813898346,
    0.7461018061, 0.5566679050, 0.2744755155, 0.1353352832,
    0.3678794412, 0.2744755155, 0.1353352832, 0.0667295910,
    0.1813898346, 0.1353352832, 0.0667295910, 0.0329022721};

static const double h4x4_lp[16] = {
    1.0000000000, 1.0000000000, 1.0000000000, 0.0000000000,
    1.0000000000, 1.0000000000, 1.0000000000, 0.0000000000,
    1.0000000000, 1.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000};

static const double h4x4_exp[16] = {
    0.0000428042, 0.0183156389, 0.5034396168, 0.0000015573,
    0.0183156389, 0.5034396168, 0.0183156389, 0.0000000001,
    0.5034396168, 0.0183156389, 0.0000000001, 0.0000000000,
    0.0000015573, 0.0000000001, 0.0000000000, 0.0000000000};

static const double h8x8_tik[64] = {
    1.0000000000, 0.9633352358, 0.8722604191, 0.7641414205, 0.6666666667,
    0.5912465769, 0.5395042868, 0.5096996438, 0.9633352358, 0.9292639906,
    0.8442331978, 0.7425456671, 0.6501695955, 0.5782345741, 0.5286491846,
    0.5000000000, 0.8722604191, 0.8442331978, 0.7734590803, 0.6872356795,
    0.6073686169, 0.5441323062, 0.5000000000, 0.4742963678, 0.7641414205,
    0.7425456671, 0.6872356795, 0.6183081408, 0.5528960384, 0.5000000000,
    0.4624894093, 0.4404126041, 0.6666666667, 0.6501695955, 0.6073686169,
    0.5528960384, 0.5000000000, 0.4563414243, 0.4248894476, 0.4061837798,
    0.5912465769, 0.5782345741, 0.5441323062, 0.5000000000, 0.4563414243,
    0.4196948644, 0.3929435038, 0.3768917873, 0.5395042868, 0.5286491846,
    0.5000000000, 0.4624894093, 0.4248894476, 0.3929435038, 0.3693980625,
    0.3551775637, 0.5096996438, 0.5000000000, 0.4742963678, 0.4404126041,
    0.4061837798, 0.3768917873, 0.3551775637, 0.3420113547};

static const double h8x8_diff[64] = {
    1.0000000000, 0.9267045621, 0.7461018061, 0.5393899137, 0.3678794412,
    0.2509043640, 0.1813898346, 0.1460392975, 0.9267045621, 0.8587813453,
    0.6914159475, 0.4998550937, 0.3409155564, 0.2325142188, 0.1680947873,
    0.1353352832, 0.7461018061, 0.6914159475, 0.5566679050, 0.4024397888,
    0.2744755155, 0.1872001991, 0.1353352832, 0.1089601836, 0.5393899137,
    0.4998550937, 0.4024397888, 0.2909414790, 0.1984304600, 0.1353352832,
    0.0978398473, 0.0787721241, 0.3678794412, 0.3409155564, 0.2744755155,
    0.1984304600, 0.1353352832, 0.0923025572, 0.0667295910, 0.0537248552,
    0.2509043640, 0.2325142188, 0.1872001991, 0.1353352832, 0.0923025572,
    0.0629529999, 0.0455115011, 0.0366418971, 0.1813898346, 0.1680947873,
    0.1353352832, 0.0978398473, 0.0667295910, 0.0455115011, 0.0329022721,
    0.0264900440, 0.1460392975, 0.1353352832, 0.1089601836, 0.0787721241,
    0.0537248552, 0.0366418971, 0.0264900440, 0.0213274764};

static const double h8x8_lp[64] = {
    1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000,
    1.0000000000, 1.0000000000, 0.0000000000, 1.0000000000, 1.0000000000,
    1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000, 0.0000000000,
    0.0000000000, 1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000,
    1.0000000000, 1.0000000000, 0.0000000000, 0.0000000000, 1.0000000000,
    1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 1.0000000000, 1.0000000000, 1.0000000000,
    1.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    1.0000000000, 1.0000000000, 1.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 1.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000};

static const double h8x8_exp[64] = {
    0.0000001125, 0.0000011719, 0.0003354626, 0.0960250916, 1.0000000000,
    0.0960250916, 0.0003354626, 0.0000011719, 0.0000011719, 0.0000101389,
    0.0017116463, 0.2223075217, 0.9114585056, 0.0344579239, 0.0000546195,
    0.0000001125, 0.0003354626, 0.0017116463, 0.0642376364, 0.8789770251,
    0.2534514477, 0.0006738783, 0.0000001125, 0.0000000001, 0.0960250916,
    0.2223075217, 0.8789770251, 0.4144332600, 0.0022489858, 0.0000001125,
    0.0000000000, 0.0000000000, 1.0000000000, 0.9114585056, 0.2534514477,
    0.0022489858, 0.0000001125, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0960250916, 0.0344579239, 0.0006738783, 0.0000001125, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0003354626, 0.0000546195,
    0.0000001125, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000011719, 0.0000001125, 0.0000000001, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000};

static const double h32_tik[32] = {
    1.0000000000, 0.9975981461, 0.9904840632, 0.9789239321, 0.9633352358,
    0.9442519611, 0.9222835942, 0.8980733668, 0.8722604191, 0.8454490709,
    0.8181867309, 0.7909504939, 0.7641414205, 0.7380849136, 0.7130354377,
    0.6891839465, 0.6666666667, 0.6455742204, 0.6259603950, 0.6078501395,
    0.5912465769, 0.5761369707, 0.5624976780, 0.5502981773, 0.5395042868,
    0.5300806941, 0.5219929172, 0.5152088010, 0.5096996438, 0.5054410303,
    0.5024134333, 0.5006026346};

static const double h32_diff[32] = {
    1.0000000000, 0.9951963015, 0.9809687064, 0.9578542387, 0.9267045621,
    0.8886260833, 0.8449055883, 0.7969291128, 0.7461018061, 0.6937755985,
    0.6411897994, 0.5894276703, 0.5393899137, 0.4917841773, 0.4471283103,
    0.4057642879, 0.3678794412, 0.3335317752, 0.3026766146, 0.2751924309,
    0.2509043640, 0.2296045640, 0.2110689898, 0.1950706879, 0.1813898346,
    0.1698209804, 0.1601779952, 0.1522972213, 0.1460392975, 0.1412900604,
    0.1379608568, 0.1359885311};

static const double h32_lp[32] = {
    1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000,
    1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000,
    1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000,
    1.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000};

static const double h32_exp[32] = {
    0.3392179834, 0.3529498475, 0.3956642735, 0.4711386395, 0.5823742119,
    0.7246980670, 0.8748564002, 0.9827768674, 0.9827768674, 0.8364047754,
    0.5800880232, 0.3150993490, 0.1295779868, 0.0392915595, 0.0086357295,
    0.0013665361, 0.0001564165, 0.0000131652, 0.0000008382, 0.0000000420,
    0.0000000017, 0.0000000001, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000};

static const double h128_tik[128] = {
    1.0000000000, 0.9998494320, 0.9993980906, 0.9986470613, 0.9975981461,
    0.9962538537, 0.9946173843, 0.9926926122, 0.9904840632, 0.9879968900,
    0.9852368433, 0.9822102404, 0.9789239321, 0.9753852655, 0.9716020470,
    0.9675825023, 0.9633352358, 0.9588691890, 0.9541935990, 0.9493179559,
    0.9442519611, 0.9390054860, 0.9335885310, 0.9280111865, 0.9222835942,
    0.9164159103, 0.9104182706, 0.9043007571, 0.8980733668, 0.8917459822,
    0.8853283444, 0.8788300281, 0.8722604191, 0.8656286937, 0.8589438003,
    0.8522144437, 0.8454490709, 0.8386558595, 0.8318427074, 0.8250172255,
    0.8181867309, 0.8113582430, 0.8045384802, 0.7977338590, 0.7909504939,
    0.7841941988, 0.7774704892, 0.7707845862, 0.7641414205, 0.7575456384,
    0.7510016071, 0.7445134220, 0.7380849136, 0.7317196557, 0.7254209728,
    0.7191919493, 0.7130354377, 0.7069540676, 0.7009502545, 0.6950262092,
    0.6891839465, 0.6834252945, 0.6777519032, 0.6721652537, 0.6666666667,
    0.6612573111, 0.6559382125, 0.6507102612, 0.6455742204, 0.6405307335,
    0.6355803321, 0.6307234430, 0.6259603950, 0.6212914260, 0.6167166893,
    0.6122362597, 0.6078501395, 0.6035582644, 0.5993605087, 0.5952566905,
    0.5912465769, 0.5873298881, 0.5835063027, 0.5797754613, 0.5761369707,
    0.5725904077, 0.5691353229, 0.5657712437, 0.5624976780, 0.5593141167,
    0.5562200369, 0.5532149047, 0.5502981773, 0.5474693057, 0.5447277368,
    0.5420729155, 0.5395042868, 0.5370212973, 0.5346233974, 0.5323100423,
    0.5300806941, 0.5279348228, 0.5258719077, 0.5238914387, 0.5219929172,
    0.5201758574, 0.5184397873, 0.5167842492, 0.5152088010, 0.5137130166,
    0.5122964870, 0.5109588205, 0.5096996438, 0.5085186021, 0.5074153598,
    0.5063896011, 0.5054410303, 0.5045693719, 0.5037743716, 0.5030557961,
    0.5024134333, 0.5018470931, 0.5013566071, 0.5009418289, 0.5006026346,
    0.5003389225, 0.5001506133, 0.5000376505};

static const double h128_diff[128] = {
    1.0000000000, 0.9996988640, 0.9987961814, 0.9972941242, 0.9951963015,
    0.9925077425, 0.9892348732, 0.9853854864, 0.9809687064, 0.9759949469,
    0.9704758635, 0.9644243017, 0.9578542387, 0.9507807219, 0.9432198026,
    0.9351884669, 0.9267045621, 0.9177867220, 0.9084542893, 0.8987272360,
    0.8886260833, 0.8781718207, 0.8673858247, 0.8562897777, 0.8449055883,
    0.8332553119, 0.8213610739, 0.8092449938, 0.7969291128, 0.7844353232,
    0.7717853014, 0.7590004437, 0.7461018061, 0.7331100476, 0.7200453771,
    0.7069275051, 0.6937755985, 0.6806082404, 0.6674433939, 0.6542983698,
    0.6411897994, 0.6281336102, 0.6151450066, 0.6022384548, 0.5894276703,
    0.5767256107, 0.5641444707, 0.5516956817, 0.5393899137, 0.5272370805,
    0.5152463482, 0.5034261456, 0.4917841773, 0.4803274393, 0.4690622359,
    0.4579941995, 0.4471283103, 0.4364689190, 0.4260197699, 0.4157840248,
    0.4057642879, 0.3959626318, 0.3863806229, 0.3770193482, 0.3678794412,
    0.3589611088, 0.3502641572, 0.3417880183, 0.3335317752, 0.3254941873,
    0.3176737156, 0.3100685463, 0.3026766146, 0.2954956272, 0.2885230847,
    0.2817563024, 0.2751924309, 0.2688284755, 0.2626613147, 0.2566877184,
    0.2509043640, 0.2453078531, 0.2398947260, 0.2346614763, 0.2296045640,
    0.2247204279, 0.2200054975, 0.2154562040, 0.2110689898, 0.2068403185,
    0.2027666833, 0.1988446146, 0.1950706879, 0.1914415301, 0.1879538256,
    0.1846043219, 0.1813898346, 0.1783072518, 0.1753535381, 0.1725257382,
    0.1698209804, 0.1672364788, 0.1647695362, 0.1624175463, 0.1601779952,
    0.1580484630, 0.1560266255, 0.1541102550, 0.1522972213, 0.1505854923,
    0.1489731347, 0.1474583146, 0.1460392975, 0.1447144485, 0.1434822327,
    0.1423412151, 0.1412900604, 0.1403275332, 0.1394524978, 0.1386639180,
    0.1379608568, 0.1373424767, 0.1368080391, 0.1363569043, 0.1359885311,
    0.1357024773, 0.1354983987, 0.1353760498};

static const double h128_lp[128] = {
    1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000,
    1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000,
    1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000,
    1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000,
    1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000,
    1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000,
    1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000,
    1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000,
    1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000,
    1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000,
    1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000,
    1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000,
    1.0000000000, 1.0000000000, 1.0000000000, 1.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000};

static const double h128_exp[128] = {
    0.2743272036, 0.2750795602, 0.2773437467, 0.2811410338, 0.2865065973,
    0.2934890666, 0.3021498158, 0.3125619192, 0.3248086687, 0.3389815299,
    0.3551773876, 0.3734949174, 0.3940299020, 0.4168693136, 0.4420839914,
    0.4697197798, 0.4997870530, 0.5322486493, 0.5670063788, 0.6038864517,
    0.6426244090, 0.6828504108, 0.7240760380, 0.7656840659, 0.8069229303,
    0.8469077876, 0.8846300948, 0.9189774547, 0.9487650166, 0.9727789577,
    0.9898314949, 0.9988255305, 0.9988255305, 0.9891297411, 0.9693375930,
    0.9394053799, 0.8996832674, 0.8509275697, 0.7942840804, 0.7312409667,
    0.6635530449, 0.5931427276, 0.5219860313, 0.4519942273, 0.3849025781,
    0.3221769093, 0.2649465683, 0.2139689501, 0.1696267687, 0.1319552914,
    0.1006934611, 0.0753506938, 0.0552803854, 0.0397517553, 0.0280133103,
    0.0193435029, 0.0130865971, 0.0086739443, 0.0056325042, 0.0035834200,
    0.0022337848, 0.0013645471, 0.0008169878, 0.0004795334, 0.0002760039,
    0.0001558266, 0.0000863287, 0.0000469497, 0.0000250768, 0.0000131610,
    0.0000067908, 0.0000034468, 0.0000017221, 0.0000008475, 0.0000004111,
    0.0000001967, 0.0000000929, 0.0000000434, 0.0000000200, 0.0000000091,
    0.0000000041, 0.0000000019, 0.0000000008, 0.0000000004, 0.0000000002,
    0.0000000001, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000,
    0.0000000000, 0.0000000000, 0.0000000000};

void mat_times_vec(const double *input, double *output, const double *gftmtx,
                   int n);

void exact_filter_4x4(const double *input, double *output, const double *h);
void exact_filter_8x8(const double *input, double *output, const double *h);
void exact_filter_32(const double *input, double *output, const double *h);
void apply_sparse_operator(const double *input, double *output, int n,
                           const int nedges, const double mev,
                           const int *adjlist, const double *wlist);
void buffer_add(const double *input, double *output, int dim, int len);
void pgf(const double *input, double *output, int n, int order,
         const double *coeffs, const int nedges, const double mev,
         const int *adjlist, const double *wlist);
void pgf_s(const double *input, double *output, int n, int order,
           const double *coeffs, const int nedges, const double mev,
           const int *adjlist, const double *wlist);
void chebyshev_gf(const double *input, double *output, int n, int order,
                  const double *coeffs, const int nedges, const double mev,
                  const int *adjlist, const double *wlist);
void armagf_cg(const double *input, double *output, int n, int tmax, int bord,
               const double *b, int aord, const double *a, const int nedges,
               const int *adjlist, const double *wlist);
void get_mpgf_terms(const int *powers, int ord, int m, int nops, int *idx_list,
                    int *pow_list);
void mpgf(const double *input, double *output, int n, int ord, int m,
          const double *coeffs, const int *idx_list, const int *pow_list,
          const int *nedges, const int *alists[], const double *wlists[]);
#endif
