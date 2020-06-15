import React, { Component } from "react";
import { View, Text, Button, TextInput, TouchableOpacity } from "react-native";

import { fetchData } from "../data/data";

import { styles } from "../assets/style/style";

export class HomeScreen extends Component {
    constructor({ props }) {
        super(props);
        this.state = { ready: false, name: "" };
    }

    componentDidMount() {
        if (!global.dataLoaded) {
            fetchData().then(() => this.setState({ ready: true }));
        } else {
            this.setState({ ready: true });
        }
    }

    render() {
        const { navigation } = this.props;

        if (this.state.ready) {
            return (
                <View style={[styles.container]}>
                    <Button
                        title="Latest Launch"
                        onPress={() => {
                            var idx = global.pastLaunches.length - 1;
                            navigation.navigate("launch", {
                                listIdx: idx,
                                name: global.launchNames[idx]
                            });
                        }}
                    />
                    <View>
                        <TextInput
                            style={{ height: 40 }}
                            placeholder="Launch name"
                            onChangeText={(name) => this.setState({ name })}
                            value={this.state.name}
                        />
                        <Button
                            title="Search"
                            onPress={() => {
                                let idx = global.launchNames.indexOf(this.state.name);
                                if (idx === -1) {
                                    alert("Launch name not found");
                                } else {
                                    navigation.navigate("launch", {
                                        listIdx: idx,
                                        name: this.state.name
                                    });
                                }
                            }}
                        />
                    </View>
                    <Button
                        title="Random Launch"
                        onPress={() => {
                            let idx = Math.floor((global.launchNames.length - 1) * Math.random());
                            navigation.navigate("launch", {
                                listIdx: idx,
                                name: global.launchNames[idx]
                            });
                        }}
                    />
                    <View>
                        <Text>Stats:</Text>
                        <View style={[styles.row]}>
                            <TouchableOpacity
                                onPress={() => navigation.navigate("launchlist", {
                                    lowerRange: 0,
                                    upperRange: global.pastLaunches.length,
                                    name: "Completed Launches"
                                })}
                                style={[styles.button]}
                            >
                                <Text style={[styles.textImportant, styles.textCenter]}>{global.pastLaunches.length}</Text>
                                <Text style={[styles.textCenter]}>completed launches</Text>
                            </TouchableOpacity>
                            <TouchableOpacity
                                onPress={() => navigation.navigate("launchlist", {
                                    lowerRange: global.pastLaunches.length,
                                    upperRange: global.launchNames.length,
                                    name: "Planned Launches"
                                })}
                                style={[styles.button]}
                            >
                                <Text style={[styles.textImportant, styles.textCenter]}>{global.upcomingLaunches.length}</Text>
                                <Text style={[styles.textCenter]}>planned launches</Text>
                            </TouchableOpacity>
                        </View>
                    </View>
                </View>
            );
        } else {
            return (
                <View>
                    <Text>Loading data</Text>
                </View>
            );
        }
    }
}