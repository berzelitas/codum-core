FROM ruban0v/eosio.cdt

ARG contract_name=mbps

ADD . /contract
WORKDIR /contract

RUN mkdir /build && eosio-cpp -o /build/${contract_name}.wasm ${contract_name}.cpp --abigen

VOLUME ["/build"]

CMD ["bash"]
