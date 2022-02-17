#/usr/bin/env bash

submission=$1
workdir=$2

team_names="xbedna74 xberdi00 xberez03 xburka00 xceska05 xdolez67 xgajdo26 xgeffe00 xgrunw00 xhavel44 xhazuc00 xholko02 xhuzde00 ximric01 xjulin08 xkanto14 xkincm02 xkinst01 xklaza00 xklemr00 xklhuf01 xklism00 xkopac06 xkoste12 xkuder04 xkunik00 xkutal09 xkyjon00 xlazur00 xmicul08 xmisak03 xmorav41 xobrus00 xpiste05 xplach09 xplesk02 xpolok03 xprasi06 xkunor00 xraino00 xrajec01 xrozpr00 xsanda03 xsarva00 xsedla1e xschne10 xsklen12 xsolar06 xsebel04 xsedym02 xsilli01 xspane04 xuhlia03 xvinar00 xvojan00 xzahor04 xzitka07"

die () {
    echo "$*" >&2
    exit 1
}

[ $# -eq 2 ] || die "Usage: SUBMISSION WORKDIR"
[ $workdir ] || die "Workdir has to be of nonzero length"


[ -f "$submission" ] || die "Submission \"$submission\" not found"

fn=$(basename -- "$submission")
ext=${fn##*.}
login=${fn%.*}

[ $ext = "zip" ] || die "Unexpected file extension \"$ext\"" 
if echo "$team_names" | grep -w "$login" > /dev/null
then
    :
else 
    die "Unknown login \"$login\"" 
fi


mkdir -p $workdir

unzip -q "$submission" -d "$workdir" || exit $?

doc_location="$workdir/$login.pdf" 
[ -f "$doc_location" ] || die "Missing documentation \"$doc_location\""

if [ -f $workdir/$login.py ] ; then
    impl=$workdir/$login.py
    grep "except\s*:" $impl && die "Found bare \`except\`"
    grep "except\s*Exception\s*:" $impl && die "Found \`except Exception\`"
    grep "except\s*BaseException\s*:" $impl && die "Found \`except BaseException\`"
elif [ -d $workdir/$login ] ; then
    impl=$workdir/$login
    grep -r "except\s*:" $impl && die "Found bare \`except\`"
    grep -r "except\s*Exception\s*:" $impl && die "Found \`except Exception\`"
    grep -r "except\s*BaseException\s*:" $impl && die "Found \`except BaseException\`"
else
    die "Missing implementation"
fi 


nb_games=10
echo "### Running $nb_games games" 

git clone -q https://github.com/ibenes/dicewars.git $workdir/repo
cp -r $impl $workdir/repo/dicewars/ai/
cd $workdir/repo
mkdir ../logs
export PYTHONPATH=$PWD:$PYTHONPATH

python3 ./scripts/dicewars-ai-only.py -r -b 11 -o 22 -s 33 -c 44 -n $nb_games -l ../logs --ai $login kb.xlogin42 dt.ste misbehaving.nop
