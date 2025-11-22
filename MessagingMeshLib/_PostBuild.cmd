rem We copy header files needed by clients...
SET header_folder="..\build-output\MessagingMeshLib\headers"
mkdir %header_folder%
copy MessagingMesh.h %header_folder%
copy Callbacks.h %header_folder%
copy Connection.h %header_folder%
copy ConnectionParams.h %header_folder%
copy BLOB.h %header_folder%
copy Field.h %header_folder%
copy Logger.h %header_folder%
copy Message.h %header_folder%
copy SharedAliases.h %header_folder%
copy Utils.h %header_folder%

