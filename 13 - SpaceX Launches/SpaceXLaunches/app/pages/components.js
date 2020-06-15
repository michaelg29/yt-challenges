import React, { Component } from "react";
import { TouchableOpacity, Image, Text, Linking, Dimensions } from "react-native";

import { styles } from "../assets/style/style";

export class Link extends Component {
    render() {
        const { text, url } = this.props;
        if (url === null) {
            return null;
        }

        return (
            <Text style={{ color: "blue" }}
                onPress={() => Linking.openURL(url)}
            >
                {text}
            </Text>
        );
    }
}

export class FullWidthImage extends Component {
    render() {
        const { uri } = this.props;
        if (uri === null) {
            return null;
        }

        const width = Dimensions.get("window").width;

        return (
            <TouchableOpacity
                onPress={() => Linking.openURL(uri)}
            >
                <Image
                    source={{ uri: uri }}
                    style={{ width: width, height: width, resizeMode: "contain" }}
                />
            </TouchableOpacity>
        );
    }
}