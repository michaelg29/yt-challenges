import React, { Component } from "react";
import { View, Text, ScrollView, FlatList } from "react-native";
import { Link, FullWidthImage } from "./components";

import { styles } from "../assets/style/style";

export class LaunchScreen extends Component {
    render() {
        const { navigation, route } = this.props;
        const { listIdx, name } = route.params;

        let item;

        if (global.nameData[name].upcoming) {
            item = global.upcomingLaunches[listIdx - global.pastLaunches.length];
        } else {
            item = global.pastLaunches[listIdx];
        }

        let listData = item["links"]["flickr_images"].map((link) => { return { key: link } });

        return (
            <View>
                <ScrollView>
                    <Text>Flight number {item["flight_number"]}</Text>
                    <Text>Launch date/time: {global.nameData[name]["date"].toLocaleString()}</Text>
                    <Text>Launch details: {item["details"]}</Text>

                    <Text>Media:</Text>

                    <Link text="Launch Video" url={item["links"]["video_link"]} />
                    <Link text="Article" url={item["links"]["article_link"]} />

                    <Text>Images:</Text>

                    <FullWidthImage uri={item["links"]["mission_patch"]} />

                    <FlatList
                        data={listData}
                        initialNumToRender={1}
                        renderItem={({ item }) => <FullWidthImage uri={item.key} />}
                    />
                </ScrollView>
            </View>
        );
    }
}