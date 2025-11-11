flatc --python $CONDA_PREFIX/share/roq/schema/api.fbs
protoc --proto_path $CONDA_PREFIX/share/roq/schema --python_out=. api.proto
