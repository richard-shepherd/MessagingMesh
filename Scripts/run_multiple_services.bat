REM We start the gateway...
pushd ..\build-output\Executables\Release
start Gateway.exe --port 5050

TIMEOUT 3

REM We start publishing to various services...
popd
pushd ..\build-output\Executables\Release\TestClient.NET\net8.0
start TestClient.NET.exe -pub-mixed -service=VULCAN
start TestClient.NET.exe -pub-mixed -service=APOLLO
start TestClient.NET.exe -pub-mixed -service=JUPITER
start TestClient.NET.exe -pub-mixed -service=MERCURY
start TestClient.NET.exe -pub-mixed -service=MINERVA
start TestClient.NET.exe -pub-mixed -service=BACCHUS
start TestClient.NET.exe -pub-mixed -service=VESTA

REM We start the Coordinator
popd
pushd ..\build-output\Executables\Release\MessagingMeshCoordinator\net8.0
start MessagingMeshCoordinator.exe

