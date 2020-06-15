import React, { Component, PureComponent } from "react";
import { View, Text, FlatList, TouchableOpacity, Image } from "react-native";

import { styles } from "../assets/style/style";

class LaunchListItem extends PureComponent {
    render() {
        const { name, navigation } = this.props;

        return (
            <TouchableOpacity
                style={[styles.listItem]}
                onPress={() => {
                    navigation.navigate("launch", {
                        listIdx: global.nameData[name]["listIdx"],
                        name: name
                    });
                }}
            >
                <Image
                    style={[styles.imageIcon, styles.listItemIcon]}
                    source={{ uri: global.nameData[name]["iconUri"] }}
                />
                <View style={[styles.listItemText]}>
                    <Text style={{ fontWeight: "bold" }}>{name}</Text>
                    <Text>{global.nameData[name]["date"].toLocaleDateString()}</Text>
                </View>
            </TouchableOpacity>
        );
    }
}

export class LaunchListScreen extends Component {
    render() {
        const { navigation, route } = this.props;
        const { lowerRange, upperRange } = route.params;
        const listNames = global.launchNames.slice(lowerRange, upperRange);
        const listData = listNames.map((name) => { return { key: name } });
        /*
            foreach name in listNames:
                listData += {
                    key: name
                }
        */

        return (
            <View style={[styles.container]}>
                <FlatList
                    data={listData}
                    initialNumToRender={8}
                    renderItem={({ item }) => <LaunchListItem
                        name={item.key}
                        navigation={navigation}
                    />}
                />
            </View>
        );
    }
}