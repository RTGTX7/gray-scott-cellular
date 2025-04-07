export CADMIUM="$HOME/gray-scott-cellular/third_party/cadmium_v2/include"
export JSON="$HOME/gray-scott-cellular/third_party/nlohmann"
if [ ! -f "$CADMIUM/cadmium/modeling/celldevs/grid/coupled.hpp" ]; then
    echo "❌ CADMIUM not found at: $CADMIUM"
    exit 1
fi

if [ ! -f "$JSON/json.hpp" ]; then
    echo "❌ json.hpp not found at: $JSON/json.hpp"
    exit 1
fi
echo "✅ CADMIUM = $CADMIUM"
echo "✅ JSON    = $JSON"

if [ -d "build" ]; then rm -Rf build; fi
mkdir -p build
cd build || exit
rm -rf *
cmake ..
make
cd ..
echo Compilation done. Executable in the bin folder